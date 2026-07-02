#include <string.h>
#include <Quickdraw.h>
#include "bitmaps.h"
#include "crabs.h"
#include "mac_compat.h"
#include "bitmaps_data.h"

struct BitmapCache {
	const Flag *flag;
	const IndexedBitmapData *data;
};

static BitmapCache caches[]={
	{ &crab_pride_flag, &crab_bitmap },
	{ &trans_crab_flag, &trans_crab_bitmap },
	{ &autistic_pride_flag, &autistic_bitmap }
};

static BitmapCache *FindCache(const Flag *flag){
	for(unsigned int i=0;i<sizeof(caches)/sizeof(caches[0]);i++){
		if(caches[i].flag==flag){
			return &caches[i];
		}
	}
	return NULL;
}

bool IsBitmapFlag(const Flag *flag){
	return FindCache(flag)!=NULL;
}

static void ByteToRGB(unsigned char r, unsigned char g, unsigned char b, RGBColor *color){
	color->red=((unsigned short)r << 8) | r;
	color->green=((unsigned short)g << 8) | g;
	color->blue=((unsigned short)b << 8) | b;
}

void DrawBitmapFlag(const Flag *flag, const Rect *destination){
	BitmapCache *cache=FindCache(flag);
	if(cache==NULL){
		EraseRect(destination);
		FrameRect(destination);
		return;
	}

	short destWidth=destination->right - destination->left;
	short destHeight=destination->bottom - destination->top;
	if(destWidth<=0 || destHeight<=0){
		return;
	}

	PenNormal();
	for(short y=0;y<destHeight;y++){
		short srcY=(short)(((long)y * cache->data->height) / destHeight);
		short destY=destination->top + y;
		const unsigned char *row=cache->data->pixels + (srcY * (cache->data->width / 2));
		short runStart=0;
		unsigned char runIndex=0xff;

		for(short x=0;x<=destWidth;x++){
			unsigned char index=0xff;
			if(x<destWidth){
				short srcX=(short)(((long)x * cache->data->width) / destWidth);
				unsigned char packed=row[srcX/2];
				index=(srcX & 1) ? (packed & 0x0f) : (packed >> 4);
			}

			if(index!=runIndex){
				if(runIndex!=0xff){
					const unsigned char *pal=cache->data->palette + runIndex * 3;
					RGBColor color;
					ByteToRGB(pal[0], pal[1], pal[2], &color);
					RGBForeColor(&color);
					MoveTo(destination->left + runStart, destY);
					LineTo(destination->left + x - 1, destY);
				}
				runStart=x;
				runIndex=index;
			}
		}
	}
}
