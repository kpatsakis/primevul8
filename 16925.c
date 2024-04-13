void gdImageCopyResampled (gdImagePtr dst, gdImagePtr src, int dstX, int dstY, int srcX, int srcY, int dstW, int dstH, int srcW, int srcH)
{
	int x, y;
	if (!dst->trueColor) {
		gdImageCopyResized (dst, src, dstX, dstY, srcX, srcY, dstW, dstH, srcW, srcH);
		return;
	}
	for (y = dstY; (y < dstY + dstH); y++) {
		for (x = dstX; (x < dstX + dstW); x++) {
			float sy1, sy2, sx1, sx2;
			float sx, sy;
			float spixels = 0.0f;
			float red = 0.0f, green = 0.0f, blue = 0.0f, alpha = 0.0f;
			float alpha_factor, alpha_sum = 0.0f, contrib_sum = 0.0f;
			sy1 = ((float)(y - dstY)) * (float)srcH / (float)dstH;
			sy2 = ((float)(y + 1 - dstY)) * (float) srcH / (float) dstH;
			sy = sy1;
			do {
				float yportion;
				if (floor_cast(sy) == floor_cast(sy1)) {
					yportion = 1.0f - (sy - floor_cast(sy));
					if (yportion > sy2 - sy1) {
						yportion = sy2 - sy1;
					}
					sy = floor_cast(sy);
				} else if (sy == floorf(sy2)) {
					yportion = sy2 - floor_cast(sy2);
				} else {
					yportion = 1.0f;
				}
				sx1 = ((float)(x - dstX)) * (float) srcW / dstW;
				sx2 = ((float)(x + 1 - dstX)) * (float) srcW / dstW;
				sx = sx1;
				do {
					float xportion;
					float pcontribution;
					int p;
					if (floorf(sx) == floor_cast(sx1)) {
						xportion = 1.0f - (sx - floor_cast(sx));
						if (xportion > sx2 - sx1) {
							xportion = sx2 - sx1;
						}
						sx = floor_cast(sx);
					} else if (sx == floorf(sx2)) {
						xportion = sx2 - floor_cast(sx2);
					} else {
						xportion = 1.0f;
					}
					pcontribution = xportion * yportion;
					p = gdImageGetTrueColorPixel(src, (int) sx + srcX, (int) sy + srcY);
					
					alpha_factor = ((gdAlphaMax - gdTrueColorGetAlpha(p))) * pcontribution;
					red += gdTrueColorGetRed (p) * alpha_factor;
					green += gdTrueColorGetGreen (p) * alpha_factor;
					blue += gdTrueColorGetBlue (p) * alpha_factor;
					alpha += gdTrueColorGetAlpha (p) * pcontribution;
					alpha_sum += alpha_factor;
					contrib_sum += pcontribution;
					spixels += xportion * yportion;
					sx += 1.0f;
				}
				while (sx < sx2);
			
				sy += 1.0f;
			}
			
			while (sy < sy2);
			
			if (spixels != 0.0f) {
				red /= spixels;
				green /= spixels;
				blue /= spixels;
				alpha /= spixels;
			}
			if ( alpha_sum != 0.0f) {
				if( contrib_sum != 0.0f) {
					alpha_sum /= contrib_sum;
				}	
				red /= alpha_sum;
				green /= alpha_sum;
				blue /= alpha_sum;
			}
			/* Clamping to allow for rounding errors above */
			if (red > 255.0f) {
				red = 255.0f;
			}
			if (green > 255.0f) {
				green = 255.0f;
			}
			if (blue > 255.0f) {
				blue = 255.0f;
			}
			if (alpha > gdAlphaMax) {
				alpha = gdAlphaMax;
			}
			gdImageSetPixel(dst, x, y, gdTrueColorAlpha ((int) red, (int) green, (int) blue, (int) alpha));
		}
	}
}