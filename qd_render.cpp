#include <Quickdraw.h>
#include <Fonts.h>
#include <TextUtils.h>
#include <string.h>
#include "qd_render.h"
#include "bitmaps.h"

static const int polygon_bigstar[]={-5,7,-3,1,-7,-2,-2,-2,0,-8,2,-2,7,-2,3,1,5,7,0,3,-1,-1,-1};
static const int polygon_tinystar[]={-3,3,-2,0,-4,-1,-1,-1,0,-4,1,-1,3,-1,1,0,2,3,0,1,-1,-1,-1};
static const int polygon_mediumstar[]={-4,6,-2,1,-6,-2,-2,-2,0,-7,1,-2,6,-2,3,1,4,6,0,2,-1,-1,-1};

static void UseColor(RGBCOLOR rgb){
	RGBColor color;
	color.red=((unsigned short)rgb.r << 8) | rgb.r;
	color.green=((unsigned short)rgb.g << 8) | rgb.g;
	color.blue=((unsigned short)rgb.b << 8) | rgb.b;
	RGBForeColor(&color);
}

static short X(const Rect *preview, int x){
	return preview->left + (short)(((long)x * (preview->right - preview->left)) / 640);
}

static short Y(const Rect *preview, int y){
	return preview->top + (short)(((long)y * (preview->bottom - preview->top)) / 480);
}

static void PaintPointList(const Rect *preview, const int *points, int pointCount, bool fill){
	PolyHandle poly=OpenPoly();
	if(poly==NULL){
		return;
	}
	MoveTo(X(preview, points[0]), Y(preview, points[1]));
	for(int i=1;i<pointCount;i++){
		LineTo(X(preview, points[i*2]), Y(preview, points[i*2+1]));
	}
	LineTo(X(preview, points[0]), Y(preview, points[1]));
	ClosePoly();
	if(fill){
		PaintPoly(poly);
	}else{
		FramePoly(poly);
	}
	KillPoly(poly);
}

static void PaintCommandPoints(const Rect *preview, const Point2D *points, int pointCount, bool fill){
	int local[16];
	for(int i=0;i<pointCount;i++){
		local[i*2]=points[i].x;
		local[i*2+1]=points[i].y;
	}
	PaintPointList(preview, local, pointCount, fill);
}

static void PaintStar(const Rect *preview, const GraphicsCommand *command){
	const int *star=NULL;
	switch(command->points[1].x){
		case BigStar:
			star=polygon_bigstar;
			break;
		case TinyStar:
			star=polygon_tinystar;
			break;
		case MediumStar:
			star=polygon_mediumstar;
			break;
		default:
			return;
	}

	int local[32];
	int count=0;
	for(int i=0;;i+=2){
		if(star[i]==-1 && star[i+1]==-1){
			break;
		}
		local[i]=star[i]+command->points[0].x;
		local[i+1]=star[i+1]+command->points[0].y;
		count++;
	}
	PaintPointList(preview, local, count, true);
}

static void DrawVectorCommand(const Rect *preview, const GraphicsCommand *command){
	Rect r;

	if(command->shape==None || command->shape==EndCommandList || command->shape==Bitmap){
		return;
	}
	if(command->shape==ShapePalette || command->shape==VGAPlane){
		return;
	}

	UseColor(command->color);
	PenNormal();

	switch(command->shape){
		case Rectangle:
			SetRect(&r, X(preview, command->points[0].x), Y(preview, command->points[0].y),
				X(preview, command->points[1].x), Y(preview, command->points[1].y));
			PaintRect(&r);
			break;
		case Triangle:
			PaintCommandPoints(preview, command->points, 3, true);
			break;
		case Quad:
			PaintCommandPoints(preview, command->points, 4, true);
			break;
		case Ellipse:
			SetRect(&r,
				X(preview, command->points[0].x-command->points[1].x),
				Y(preview, command->points[0].y-command->points[1].y),
				X(preview, command->points[0].x+command->points[1].x),
				Y(preview, command->points[0].y+command->points[1].y));
			PaintOval(&r);
			break;
		case ShapePolygon:
			PaintPointList(preview, command->point_list, command->point_count, true);
			break;
		case ShapePolygonOutline:
			PaintPointList(preview, command->point_list, command->point_count, false);
			break;
		case ThickShapePolygonOutline:
			PenSize(3, 3);
			PaintPointList(preview, command->point_list, command->point_count, false);
			PenNormal();
			break;
		case Lines:
		case ThickLines:
			if(command->shape==ThickLines){
				PenSize(3, 3);
			}
			for(int i=0;i<command->point_count-1;i++){
				int offset=i*2;
				MoveTo(X(preview, command->point_list[offset]), Y(preview, command->point_list[offset+1]));
				LineTo(X(preview, command->point_list[offset+2]), Y(preview, command->point_list[offset+3]));
			}
			PenNormal();
			break;
		case Star:
			PaintStar(preview, command);
			break;
		default:
			break;
	}
}

static void DrawFlagTitle(const Flag *flag, const Rect *preview){
	char buffer[200];
	const char *stop;
	short x;

	strcpy(buffer, flag->name);
	stop=strpbrk(buffer, ".,:");
	if(stop!=NULL){
		*((char *)stop)=0;
	}

	UseColor(flag->text_color);
	TextFont(3);
	TextSize(flag->text_size<=2 ? 12 : flag->text_size<=3 ? 14 : 18);

	if(flag->text_layout==LAYOUT_RIGHT){
		x=preview->right - 20;
		MoveTo(x - TextWidth(buffer, 0, strlen(buffer)), Y(preview, 460));
	}else if(flag->text_layout==LAYOUT_LEFT){
		MoveTo(preview->left + 20, Y(preview, 460));
	}else{
		short width=TextWidth(buffer, 0, strlen(buffer));
		MoveTo(X(preview, 320) - width/2, Y(preview, 460));
	}
	DrawText(buffer, 0, strlen(buffer));
}

void DrawFlag(const Flag *flag, const Rect *previewRect, bool drawTitle){
	Rect clip=*previewRect;
	ClipRect(&clip);
	Rect erase=*previewRect;
	EraseRect(&erase);

	if(IsBitmapFlag(flag)){
		DrawBitmapFlag(flag, previewRect);
	}else{
		for(int i=0;i<MAX_COMMANDS;i++){
			const GraphicsCommand *command=&flag->commands[i];
			if(command->shape==EndCommandList){
				break;
			}
			DrawVectorCommand(previewRect, command);
		}
	}

	if(drawTitle){
		DrawFlagTitle(flag, previewRect);
	}
	ClipRect(&qd.screenBits.bounds);
}

void DrawFlagCredit(const Flag *flag, const Rect *creditRect){
	EraseRect(creditRect);
	if(flag->credit==NULL){
		return;
	}
	RGBColor black={0,0,0};
	RGBForeColor(&black);
	TextFont(3);
	TextSize(10);
	MoveTo(creditRect->left, creditRect->top + 12);
	DrawText((Ptr)flag->credit, 0, strlen(flag->credit));
}
