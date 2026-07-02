#ifndef MAC_COMPAT_H
#define MAC_COMPAT_H

#include <Quickdraw.h>
#include <Windows.h>

static inline BitMap *WindowBits(WindowPtr window){
#if TARGET_API_MAC_CARBON
	return GetPortBitMapForCopyBits(GetWindowPort(window));
#else
	return &window->portBits;
#endif
}

static inline void SetWindowPortCompat(WindowPtr window){
#if TARGET_API_MAC_CARBON
	SetPortWindowPort(window);
#else
	SetPort(window);
#endif
}

static inline Rect WindowPortRect(WindowPtr window){
	Rect r;
#if TARGET_API_MAC_CARBON
	GetPortBounds(GetWindowPort(window), &r);
#else
	r=window->portRect;
#endif
	return r;
}

#endif
