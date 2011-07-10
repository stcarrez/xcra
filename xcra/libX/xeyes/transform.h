/*
 * header file for transformed coordinate system.  No rotations
 * supported, as elipses cannot be rotated in X.
 */

#ifndef __P
# ifdef __STDC__
#  define __P(x)	x
# else
#  define __P(x)	()
# endif
#endif

typedef struct _transform {
	double	mx, bx;
	double	my, by;
} Transform;

typedef struct _TPoint {
	double	x, y;
} TPoint;

typedef struct _TRectangle {
	double	x, y, width, height;
} TRectangle;

# define Xx(x,y,t)	((int)((t)->mx * (x) + (t)->bx + 0.5))
# define Xy(x,y,t)	((int)((t)->my * (y) + (t)->by + 0.5))
# define Xwidth(w,h,t)	((int)((t)->mx * (w) + 0.5))
# define Xheight(w,h,t)	((int)((t)->my * (h) + 0.5))
# define Tx(x,y,t)	((((double) (x)) - (t)->bx) / (t)->mx)
# define Ty(x,y,t)	((((double) (y)) - (t)->by) / (t)->my)
# define Twidth(w,h,t)	(((double) (w)) / (t)->mx)
# define Theight(w,h,t)	(((double) (h)) / (t)->my)

extern void DrawArc	__P((Display*, Drawable, GC, Transform*,
			     double x, double y, double width,
			     double height, int angle1, int angle2));
extern void TFillArc	__P((Display*, Drawable, GC, Transform*,
			     double x, double y, double width,
			     double height, int angle1, int angle2));
extern void SetTransform __P((Transform*, int, int, int, int,
			      double, double, double, double));
