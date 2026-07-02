#ifndef SELECTOR_H
#define SELECTOR_H

#include <Quickdraw.h>
#include <Events.h>
#include <Windows.h>
#include "flags.h"

void InitSelector(WindowPtr window);
void DrawSelector(WindowPtr window);
bool HandleSelectorEvent(WindowPtr window, const EventRecord *event);
Flag *GetSelectedFlag();

#endif
