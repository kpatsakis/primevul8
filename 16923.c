gdImagePolygon (gdImagePtr im, gdPointPtr p, int n, int c)
{
	int i;
	int lx, ly;
	typedef void (*image_line)(gdImagePtr im, int x1, int y1, int x2, int y2, int color);
	image_line draw_line;

	if (!n) {
		return;
	}
	if ( im->antialias) {
		draw_line = gdImageAALine;
	} else {
		draw_line = gdImageLine;
	}
	lx = p->x;
	ly = p->y;
	draw_line(im, lx, ly, p[n - 1].x, p[n - 1].y, c);
	for (i = 1; (i < n); i++) {
		p++;
		draw_line(im, lx, ly, p->x, p->y, c);
		lx = p->x;
		ly = p->y;
	}
}