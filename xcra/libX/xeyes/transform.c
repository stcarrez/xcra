/*
 * transformed coordinate system objects for X
 */

# include	<X11/Xlib.h>
# include	"transform.h"
# include	<stdlib.h>

void
TDrawArc (dpy, d, gc, t, x, y, width, height, angle1, angle2)
	register Display	*dpy;
	Drawable		d;
	GC			gc;
	Transform		*t;
	double			x, y, width, height;
	int			angle1, angle2;
{
	int	xx, xy, xw, xh;

	xx = Xx(x,y,t);
	xy = Xy(x,y,t);
	xw = Xwidth (width, height, t);
	xh = Xheight (width, height, t);
	if (xw < 0) {
		xx += xw;
		xw = -xw;
	}
	if (xh < 0) {
		xy += xh;
		xh = -xh;
	}
	XDrawArc (dpy, d, gc, xx, xy, xw, xh, angle1, angle2);
}

void
TFillArc (dpy, d, gc, t, x, y, width, height, angle1, angle2)
	register Display	*dpy;
	Drawable		d;
	GC			gc;
	Transform		*t;
	double			x, y, width, height;
	int			angle1, angle2;
{
	int	xx, xy, xw, xh;

	xx = Xx(x,y,t);
	xy = Xy(x,y,t);
	xw = Xwidth (width, height, t);
	xh = Xheight (width, height, t);
	if (xw < 0) {
		xx += xw;
		xw = -xw;
	}
	if (xh < 0) {
		xy += xh;
		xh = -xh;
	}
	XFillArc (dpy, d, gc, xx, xy, xw, xh, angle1, angle2);
}

void
SetTransform (t, xx1, xx2, xy1, xy2, tx1, tx2, ty1, ty2)
Transform	*t;
int		xx1, xx2, xy1, xy2;
double		tx1, tx2, ty1, ty2;
{
	t->mx = ((double) xx2 - xx1) / (tx2 - tx1);
	t->bx = ((double) xx1) - t->mx * tx1;
	t->my = ((double) xy2 - xy1) / (ty2 - ty1);
	t->by = ((double) xy1) - t->my * ty1;
}
