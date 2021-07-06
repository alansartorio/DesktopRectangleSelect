#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <stdio.h>
#include <stdlib.h>


Display *dis;
int screen;
Window win;
GC gc;
Colormap colormap;

Atom wm_delete_window;

void init_x(XID winID) {
	unsigned long transparent = 0;
	
	dis = XOpenDisplay((char *)0);
	screen = DefaultScreen(dis);
	//black = BlackPixel(dis, screen);	
	//white = WhitePixel(dis, screen);

	XVisualInfo vinfo;
	XMatchVisualInfo(dis, DefaultScreen(dis), 32, TrueColor, &vinfo);
	XSetWindowAttributes attr = { 0 };
	attr.colormap = colormap = XCreateColormap(dis, DefaultRootWindow(dis), vinfo.visual, AllocNone);
	attr.border_pixel = 0;
	attr.background_pixel = 0x0;
	attr.event_mask = ExposureMask|ButtonPressMask|ButtonReleaseMask|Button1MotionMask|KeyPressMask;
	unsigned long attrMask = CWColormap | CWBorderPixel | CWBackPixel | CWEventMask;
	XClassHint hint = {
		.res_class = "DesktopRectangle",
		.res_name = "desktoprectangle"
	};

	if (winID == 0) {
		win = XCreateWindow(dis, DefaultRootWindow(dis), 0, 0, 300, 200, 0, vinfo.depth, InputOutput, vinfo.visual, attrMask, &attr);
		XSetStandardProperties(dis, win, "Desktop Rectangle Selection", "Desktop Rectangle Selection", None, NULL, 0, NULL);
		XSetClassHint(dis, win, &hint);
		
		wm_delete_window = XInternAtom(dis, "WM_DELETE_WINDOW", False);
		XSetWMProtocols(dis, win, &wm_delete_window, 1);
		XMapWindow(dis, win);
	
		XClearWindow(dis, win);
		XMapRaised(dis, win);

	} else {
		win = (Window) winID;
		XSetClassHint(dis, win, &hint);
		XChangeWindowAttributes(dis, win, attrMask, &attr);
	}



	XGCValues gcValues = { 0 };
	gcValues.line_width = 4;
	gcValues.cap_style = CapRound;
	gcValues.line_style = LineSolid;

	gc = XCreateGC(dis, win, GCLineWidth | GCCapStyle | GCLineStyle, &gcValues);
	XClearWindow(dis, win);
}

void close_x() {
	XFreeGC(dis, gc);
	XDestroyWindow(dis, win);
	XCloseDisplay(dis);
	exit(1);
}

void redraw() {
	XClearWindow(dis, win);
}

typedef struct {
	int x, y;
} Position;

unsigned long RGB(int r, int g, int b, int alpha) {
	return (b * alpha / 255) + (g * alpha / 255 << 8) + (r * alpha / 255 << 16) + (alpha << 24);
	//return b + (g << 8) + (r << 16);
}

int main(int argc, char **argv) {

	XID winID = 0;
	if (argc == 2) {
		printf("Arg: %s\n", argv[1]);
		winID = strtol(argv[1], NULL, 0);
		printf("Running on window with ID=0x%lx\n", winID);
	}
	init_x(winID);

	XEvent event;
	Position pressed;

	unsigned long rectangleBorder = RGB(0x22, 0x22, 0xAA, 0xFF);
	unsigned long rectangleFill = RGB(0x55, 0x55, 0xFF, 0x88);


	while (1) {
		XNextEvent(dis, &event);
		if (event.type == Expose && event.xexpose.count == 0) {
			redraw();
		}
		if (event.type == ClientMessage) {
			if ((Atom)event.xclient.data.l[0] == wm_delete_window) {
				close_x();
			}
		}
		if (event.type == ButtonPress) {
			//printf("You pressed the mouse button at: (%d, %d)\n", event.xbutton.x, event.xbutton.y);
			pressed = (Position) {.x = event.xbutton.x, .y = event.xbutton.y };
		}
		if (event.type == ButtonRelease) {
			XClearWindow(dis, win);
		}
		if (event.type == MotionNotify) {
			XClearWindow(dis, win);
			Position current = (Position) {.x = event.xbutton.x, .y = event.xbutton.y };
			Position start = { .x = pressed.x < current.x ? pressed.x : current.x, .y = pressed.y < current.y ? pressed.y : current.y };
			int width = abs(pressed.x - current.x);
			int height = abs(pressed.y - current.y);

			//XSetForeground(dis, gc, rectangleColor.pixel);
			XSetForeground(dis, gc, rectangleFill);
			XFillRectangle(dis, win, gc, start.x, start.y, width, height);
			XSetForeground(dis, gc, rectangleBorder);
			XDrawRectangle(dis, win, gc, start.x, start.y, width, height);
			XSync(dis, False);
			//printf("Moved. \n");
		}
	}

	close_x();

	return 0;
}
