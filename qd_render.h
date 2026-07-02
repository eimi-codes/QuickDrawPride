#ifndef QD_RENDER_H
#define QD_RENDER_H

#include <Quickdraw.h>
#include "flags.h"

void DrawFlag(const Flag *flag, const Rect *previewRect, bool drawTitle);
void DrawFlagCredit(const Flag *flag, const Rect *creditRect);

#endif
