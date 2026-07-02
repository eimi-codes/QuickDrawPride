#include <Quickdraw.h>
#include <Fonts.h>
#include <TextUtils.h>
#include <string.h>
#include <ctype.h>
#include "selector.h"
#include "qd_render.h"
#include "mac_compat.h"

static const short kListLeft=10;
static const short kListTop=10;
static const short kPreviewTop=10;
static const short kRowHeight=16;
static const short kMargin=10;
static const short kCreditHeight=18;
static const short kNominalListWidth=190;
static const short kNarrowListWidth=160;
static const short kMinPreviewWidth=320;
static const short kMinPreviewHeight=240;

struct SelectorLayout {
	Rect list;
	Rect preview;
	Rect credit;
};

static WindowPtr selectorWindow=NULL;
static int selectedIndex=0;
static int scrollTop=0;
static int flagCount=0;
static char typeBuffer[64];
static unsigned long lastTypeTick=0;

static int CountFlags(){
	int count=0;
	while(PRIDE_FLAGS[count]!=NULL){
		count++;
	}
	return count;
}

static void GetLayout(SelectorLayout *layout){
	Rect content=WindowPortRect(selectorWindow);
	short contentWidth=content.right - content.left;
	short listWidth=contentWidth<760 ? kNarrowListWidth : kNominalListWidth;
	short previewLeft=kListLeft + listWidth + kMargin;
	short availableWidth=content.right - previewLeft - kMargin;
	short availableHeight=content.bottom - kPreviewTop - kCreditHeight - (kMargin * 2);
	short previewWidth;
	short previewHeight;

	if(availableWidth<kMinPreviewWidth){
		availableWidth=kMinPreviewWidth;
	}
	if(availableHeight<kMinPreviewHeight){
		availableHeight=kMinPreviewHeight;
	}

	previewWidth=availableWidth;
	previewHeight=(short)(((long)previewWidth * 3) / 4);
	if(previewHeight>availableHeight){
		previewHeight=availableHeight;
		previewWidth=(short)(((long)previewHeight * 4) / 3);
	}

	SetRect(&layout->list, kListLeft, kListTop, kListLeft+listWidth, kListTop+previewHeight);
	SetRect(&layout->preview, previewLeft, kPreviewTop, previewLeft+previewWidth, kPreviewTop+previewHeight);
	SetRect(&layout->credit, previewLeft, layout->preview.bottom+6, previewLeft+previewWidth, layout->preview.bottom+6+kCreditHeight);
}

static int VisibleRows(){
	SelectorLayout layout;
	GetLayout(&layout);
	return ((layout.list.bottom - layout.list.top) / kRowHeight);
}

static void EnsureSelectionVisible(){
	int rows=VisibleRows();
	if(selectedIndex<scrollTop){
		scrollTop=selectedIndex;
	}
	if(selectedIndex>=scrollTop+rows){
		scrollTop=selectedIndex-rows+1;
	}
	if(scrollTop<0){
		scrollTop=0;
	}
	if(scrollTop>flagCount-rows){
		scrollTop=flagCount-rows;
	}
	if(scrollTop<0){
		scrollTop=0;
	}
}

static void DrawScrollbarHint(const Rect *list){
	Rect track=*list;
	track.left=track.right-8;
	RGBColor gray={0x9999,0x9999,0x9999};
	RGBForeColor(&gray);
	PaintRect(&track);
	if(flagCount<=VisibleRows()){
		return;
	}

	Rect thumb=track;
	int rows=VisibleRows();
	int trackHeight=track.bottom-track.top;
	int thumbHeight=(trackHeight*rows)/flagCount;
	if(thumbHeight<18){
		thumbHeight=18;
	}
	thumb.top=track.top + ((trackHeight-thumbHeight)*scrollTop)/(flagCount-rows);
	thumb.bottom=thumb.top + thumbHeight;
	RGBColor dark={0x4444,0x4444,0x4444};
	RGBForeColor(&dark);
	PaintRect(&thumb);
}

static void DrawList(){
	SelectorLayout layout;
	Rect list;
	GetLayout(&layout);
	list=layout.list;
	EraseRect(&list);
	FrameRect(&list);
	TextFont(3);
	TextSize(10);

	int rows=VisibleRows();
	for(int row=0;row<rows;row++){
		int index=scrollTop+row;
		if(index>=flagCount){
			break;
		}
		Rect rowRect;
		SetRect(&rowRect, list.left+1, list.top+1+row*kRowHeight, list.right-9, list.top+1+(row+1)*kRowHeight);
		if(index==selectedIndex){
			InvertRect(&rowRect);
		}
		MoveTo(rowRect.left+4, rowRect.top+12);
		DrawText((Ptr)PRIDE_FLAGS[index]->name, 0, strlen(PRIDE_FLAGS[index]->name));
	}
	DrawScrollbarHint(&list);
}

static void DrawPreview(){
	SelectorLayout layout;
	GetLayout(&layout);
	DrawFlag(GetSelectedFlag(), &layout.preview, false);
	FrameRect(&layout.preview);
	DrawFlagCredit(GetSelectedFlag(), &layout.credit);
}

static bool StartsWithFolded(const char *text, const char *prefix){
	while(*prefix){
		if(tolower((unsigned char)*text)!=tolower((unsigned char)*prefix)){
			return false;
		}
		text++;
		prefix++;
	}
	return true;
}

static void SelectIndex(int index){
	if(index<0){
		index=0;
	}
	if(index>=flagCount){
		index=flagCount-1;
	}
	if(index==selectedIndex){
		return;
	}
	selectedIndex=index;
	EnsureSelectionVisible();
	DrawSelector(selectorWindow);
}

static void TypeSelect(char ch){
	unsigned long now=TickCount();
	if(now-lastTypeTick>60){
		typeBuffer[0]=0;
	}
	lastTypeTick=now;

	int len=strlen(typeBuffer);
	if(len<(int)sizeof(typeBuffer)-1 && ch>=32 && ch<127){
		typeBuffer[len]=ch;
		typeBuffer[len+1]=0;
	}
	if(typeBuffer[0]==0){
		return;
	}

	for(int offset=0;offset<flagCount;offset++){
		int index=(selectedIndex+offset)%flagCount;
		if(StartsWithFolded(PRIDE_FLAGS[index]->name, typeBuffer)){
			SelectIndex(index);
			return;
		}
	}
}

void InitSelector(WindowPtr window){
	selectorWindow=window;
	flagCount=CountFlags();
	selectedIndex=0;
	scrollTop=0;
	typeBuffer[0]=0;
}

void DrawSelector(WindowPtr window){
	if(window==NULL){
		return;
	}
	SetWindowPortCompat(window);
	Rect content=WindowPortRect(window);
	EraseRect(&content);
	DrawList();
	DrawPreview();
}

bool HandleSelectorEvent(WindowPtr window, const EventRecord *event){
	if(window==NULL || event==NULL){
		return false;
	}
	SetWindowPortCompat(window);

	if(event->what==keyDown || event->what==autoKey){
		char ch=event->message & charCodeMask;
		char key=(event->message & keyCodeMask) >> 8;
		if(ch==0x1e || key==0x7e){
			SelectIndex(selectedIndex-1);
			return true;
		}
		if(ch==0x1f || key==0x7d){
			SelectIndex(selectedIndex+1);
			return true;
		}
		if(ch==0x0b || key==0x74){
			SelectIndex(selectedIndex-VisibleRows());
			return true;
		}
		if(ch==0x0c || key==0x79){
			SelectIndex(selectedIndex+VisibleRows());
			return true;
		}
		if(ch>=32 && ch<127){
			TypeSelect(ch);
			return true;
		}
	}

	if(event->what==mouseDown){
		Point p=event->where;
		GlobalToLocal(&p);
		SelectorLayout layout;
		GetLayout(&layout);
		if(PtInRect(p, &layout.list)){
			int row=(p.v-layout.list.top-1)/kRowHeight;
			SelectIndex(scrollTop+row);
			return true;
		}
	}

	return false;
}

Flag *GetSelectedFlag(){
	if(flagCount==0){
		return NULL;
	}
	return PRIDE_FLAGS[selectedIndex];
}
