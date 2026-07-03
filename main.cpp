#include <Quickdraw.h>
#include <Windows.h>
#include <Menus.h>
#include <Fonts.h>
#include <TextEdit.h>
#include <Dialogs.h>
#include <Events.h>
#include <Devices.h>
#include <ToolUtils.h>
#include <TextUtils.h>
#include <string.h>
#include "selector.h"
#include "mac_compat.h"

static const short kAppleMenu=128;
static const short kFileMenu=129;
static const short kAboutItem=1;
static const short kQuitItem=1;

static WindowPtr mainWindow=NULL;
static bool running=true;

static void CToP(const char *c, unsigned char *p){
	size_t len=strlen(c);
	if(len>255){
		len=255;
	}
	p[0]=len;
	memcpy(p+1, c, len);
}

static void InstallMenus(){
	MenuHandle apple=NewMenu(kAppleMenu, "\p\024");
	AppendMenu(apple, "\pAbout QuickDrawPride...");
	AppendResMenu(apple, 'DRVR');
	InsertMenu(apple, 0);

	MenuHandle file=NewMenu(kFileMenu, "\pFile");
	AppendMenu(file, "\pQuit/Q");
	InsertMenu(file, 0);

	DrawMenuBar();
}

static void DrawAboutText(WindowPtr window){
	SetWindowPortCompat(window);
	Rect r=WindowPortRect(window);
	EraseRect(&r);
	TextFont(3);
	TextSize(12);

	MoveTo(18, 28);
	DrawString("\pQuickDrawPride");
	MoveTo(18, 50);
	DrawString("\pA classic Mac OS port of Alice Averlong's VGAPride.");
	MoveTo(18, 72);
	DrawString("\pVector flags render natively through QuickDraw.");
	MoveTo(18, 94);
	DrawString("\pCrab and Autistic Pride artwork is regenerated from source PNGs.");
	MoveTo(18, 124);
	DrawString("\pCredits and licenses are preserved from upstream.");
	MoveTo(18, 146);
	DrawString("\pGPL-3.0 port; Autistic Pride asset CC BY-SA 4.0.");
	MoveTo(18, 184);
	DrawString("\pClick or press any key to close.");
}

static void ShowAbout(){
	Rect r;
	SetRect(&r, 160, 120, 660, 330);
	WindowPtr about=NewCWindow(NULL, &r, "\pAbout QuickDrawPride", true, dBoxProc, (WindowPtr)-1, false, 0);
	if(about==NULL){
		return;
	}
	DrawAboutText(about);

	bool done=false;
	while(!done){
		EventRecord event;
		SystemTask();
		if(WaitNextEvent(everyEvent, &event, 20, NULL)){
			switch(event.what){
				case keyDown:
				case mouseDown:
					done=true;
					break;
				case updateEvt:
					if((WindowPtr)event.message==about){
						BeginUpdate(about);
						DrawAboutText(about);
						EndUpdate(about);
					}
					break;
			}
		}
	}
	DisposeWindow(about);
	if(mainWindow!=NULL){
		DrawSelector(mainWindow);
	}
}

static void DoMenuCommand(long choice){
	short menu=(choice >> 16) & 0xffff;
	short item=choice & 0xffff;

	if(menu==kAppleMenu && item==kAboutItem){
		ShowAbout();
	}else if(menu==kFileMenu && item==kQuitItem){
		running=false;
	}
	HiliteMenu(0);
}

static void MakeMainWindow(){
	Rect screen=qd.screenBits.bounds;
	Rect r;
	short left=screen.left+20;
	short top=screen.top+40;
	short right=left+860;
	short bottom=top+540;
	if(right>screen.right-10){
		right=screen.right-10;
	}
	if(bottom>screen.bottom-10){
		bottom=screen.bottom-10;
	}
	SetRect(&r, left, top, right, bottom);
	mainWindow=NewCWindow(NULL, &r, "\pQuickDrawPride", true, documentProc, (WindowPtr)-1, true, 0);
	InitSelector(mainWindow);
	DrawSelector(mainWindow);
}

static void HandleKey(const EventRecord *event){
	if(event->modifiers & cmdKey){
		DoMenuCommand(MenuKey(event->message & charCodeMask));
		return;
	}
	char ch=event->message & charCodeMask;
	if(ch==0x1b){
		running=false;
		return;
	}
	HandleSelectorEvent(mainWindow, event);
}

static void HandleMouseDown(EventRecord *event){
	WindowPtr window;
	switch(FindWindow(event->where, &window)){
		case inMenuBar:
			DoMenuCommand(MenuSelect(event->where));
			break;
		case inDrag:
			DragWindow(window, event->where, &qd.screenBits.bounds);
			break;
		case inGrow:
			if(window==mainWindow){
				Rect screen=qd.screenBits.bounds;
				Rect limits;
				long size;
				SetRect(&limits, 520, 360, screen.right-screen.left-20, screen.bottom-screen.top-20);
				size=GrowWindow(window, event->where, &limits);
				if(size!=0){
					SizeWindow(window, (short)(size & 0xffff), (short)((size >> 16) & 0xffff), true);
					DrawSelector(mainWindow);
				}
			}
			break;
		case inGoAway:
			if(window==mainWindow && TrackGoAway(window, event->where)){
				running=false;
			}
			break;
		case inContent:
			if(window!=FrontWindow()){
				SelectWindow(window);
			}else if(window==mainWindow){
				HandleSelectorEvent(mainWindow, event);
			}
			break;
		case inSysWindow:
			SystemClick(event, window);
			break;
	}
}

int main(){
	InitGraf(&qd.thePort);
	InitFonts();
	InitWindows();
	InitMenus();
	TEInit();
	InitDialogs(NULL);
	InitCursor();

	InstallMenus();
	MakeMainWindow();

	while(running){
		EventRecord event;
		SystemTask();
		if(WaitNextEvent(everyEvent, &event, 20, NULL)){
			switch(event.what){
				case mouseDown:
					HandleMouseDown(&event);
					break;
				case keyDown:
				case autoKey:
					HandleKey(&event);
					break;
				case updateEvt:
					if((WindowPtr)event.message==mainWindow){
						BeginUpdate(mainWindow);
						DrawSelector(mainWindow);
						EndUpdate(mainWindow);
					}
					break;
				case activateEvt:
					if(mainWindow!=NULL){
						DrawSelector(mainWindow);
					}
					break;
			}
		}
	}

	return 0;
}
