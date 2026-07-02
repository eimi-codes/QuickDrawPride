#ifndef BITMAPS_H
#define BITMAPS_H

#include <Quickdraw.h>
#include "flags.h"

bool IsBitmapFlag(const Flag *flag);
void DrawBitmapFlag(const Flag *flag, const Rect *destination);

#endif
