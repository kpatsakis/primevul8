gdImageCopyMergeGray (gdImagePtr dst, gdImagePtr src, int dstX, int dstY, int srcX, int srcY, int w, int h, int pct)
{
	int c, dc;
	int x, y;
	int tox, toy;
	int ncR, ncG, ncB;
	float g;
	toy = dstY;

	for (y = srcY; (y < (srcY + h)); y++) {
		tox = dstX;
		for (x = srcX; (x < (srcX + w)); x++) {
			int nc;
			c = gdImageGetPixel (src, x, y);
			/* Added 7/24/95: support transparent copies */
			if (gdImageGetTransparent(src) == c) {
				tox++;
				continue;
			}
			/* If it's the same image, mapping is trivial */
			if (dst == src) {
				nc = c;
			} else {
				dc = gdImageGetPixel(dst, tox, toy);
				g = (0.29900f * gdImageRed(dst, dc)) + (0.58700f * gdImageGreen(dst, dc)) + (0.11400f * gdImageBlue(dst, dc));

				ncR = (int)(gdImageRed (src, c) * (pct / 100.0f) + gdImageRed(dst, dc) * g * ((100 - pct) / 100.0f));
				ncG = (int)(gdImageGreen (src, c) * (pct / 100.0f) + gdImageGreen(dst, dc) * g * ((100 - pct) / 100.0f));
				ncB = (int)(gdImageBlue (src, c) * (pct / 100.0f) + gdImageBlue(dst, dc) * g * ((100 - pct) / 100.0f));

				/* First look for an exact match */
				nc = gdImageColorExact(dst, ncR, ncG, ncB);
				if (nc == (-1)) {
					/* No, so try to allocate it */
					nc = gdImageColorAllocate(dst, ncR, ncG, ncB);
					/* If we're out of colors, go for the closest color */
					if (nc == (-1)) {
						nc = gdImageColorClosest(dst, ncR, ncG, ncB);
					}
				}
			}
			gdImageSetPixel(dst, tox, toy, nc);
			tox++;
		}
		toy++;
	}
}