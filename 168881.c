BGD_DECLARE(int) gdImagePaletteToTrueColor(gdImagePtr src)
{
	unsigned int y;
	unsigned int yy;

	if (src == NULL) {
		return 0;
	}

	if (src->trueColor == 1) {
		return 1;
	} else {
		unsigned int x;
		const unsigned int sy = gdImageSY(src);
		const unsigned int sx = gdImageSX(src);

		src->tpixels = (int **) gdMalloc(sizeof(int *) * sy);
		if (src->tpixels == NULL) {
			return 0;
		}

		for (y = 0; y < sy; y++) {
			const unsigned char *src_row = src->pixels[y];
			int * dst_row;

			/* no need to calloc it, we overwrite all pxl anyway */
			src->tpixels[y] = (int *) gdMalloc(sx * sizeof(int));
			if (src->tpixels[y] == NULL) {
				goto clean_on_error;
			}

			dst_row = src->tpixels[y];
			for (x = 0; x < sx; x++) {
				const unsigned char c = *(src_row + x);
				if (c == src->transparent) {
					*(dst_row + x) = gdTrueColorAlpha(0, 0, 0, 127);
				} else {
					*(dst_row + x) = gdTrueColorAlpha(src->red[c], src->green[c], src->blue[c], src->alpha[c]);
				}
			}
		}
	}

	/* free old palette buffer (y is sy) */
	for (yy = 0; yy < y; yy++) {
		gdFree(src->pixels[yy]);
	}
	gdFree(src->pixels);
	src->trueColor = 1;
	src->pixels = NULL;
	src->alphaBlendingFlag = 0;
	src->saveAlphaFlag = 1;
	return 1;

clean_on_error:
	/* free new true color buffer (y is not allocated, have failed) */
	for (yy = 0; yy < y; yy++) {
		gdFree(src->tpixels[yy]);
	}
	gdFree(src->tpixels);
	return 0;
}