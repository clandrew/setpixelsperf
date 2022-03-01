# Performance Experiment- Comparing GDI SetPixels and SetDIBits

This is a very simple, minimalistic benchmark program for comparing
*	SetPixel, plotting each pixel of a window sequentially one by one, against
*	SetDIBits, where all pixels of a window are set from memory at once.

On each draw, the plots use a different color so that you know the clear is happening.

![Example image](https://raw.githubusercontent.com/clandrew/setpixelsperf/main/Screenshot.png "Example image.")

Nothing too fancy.

