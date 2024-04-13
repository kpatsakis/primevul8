static inline void cached_cmyk_conv(unsigned char *restrict const pr, unsigned char *restrict const pg, unsigned char *restrict const pb,
				unsigned int *restrict const C, unsigned int *restrict const M, unsigned int *restrict const Y, unsigned int *restrict const K,
				unsigned int c, unsigned int m, unsigned int y, unsigned int k)
{
#ifdef SLOWCMYK
	unsigned int r, g, b;
	unsigned int cm, c1m, cm1, c1m1, c1m1y, c1m1y1, c1my, c1my1, cm1y, cm1y1, cmy, cmy1;
	unsigned int x0, x1;

	if (c == *C && m == *M && y == *Y && k == *K)
	{
		/* Nothing to do */
	}
	else if (k == 0 && c == 0 && m == 0 && y == 0)
	{
		*C = 0;
		*M = 0;
		*Y = 0;
		*K = 0;
		*pr = *pg = *pb = 255;
	}
	else if (k == 255)
	{
		*C = 0;
		*M = 0;
		*Y = 0;
		*K = 255;
		*pr = *pg = *pb = 0;
	}
	else
	{
		*C = c;
		*M = m;
		*Y = y;
		*K = k;
		c += c>>7;
		m += m>>7;
		y += y>>7;
		k += k>>7;
		y >>= 1; /* Ditch 1 bit of Y to avoid overflow */
		cm = c * m;
		c1m = (m<<8) - cm;
		cm1 = (c<<8) - cm;
		c1m1 = ((256 - m)<<8) - cm1;
		c1m1y = c1m1 * y;
		c1m1y1 = (c1m1<<7) - c1m1y;
		c1my = c1m * y;
		c1my1 = (c1m<<7) - c1my;
		cm1y = cm1 * y;
		cm1y1 = (cm1<<7) - cm1y;
		cmy = cm * y;
		cmy1 = (cm<<7) - cmy;

		/* this is a matrix multiplication, unrolled for performance */
		x1 = c1m1y1 * k;	/* 0 0 0 1 */
		x0 = (c1m1y1<<8) - x1;	/* 0 0 0 0 */
		x1 = x1>>8;		/* From 23 fractional bits to 15 */
		r = g = b = x0;
		r += 35 * x1;	/* 0.1373f */
		g += 31 * x1;	/* 0.1216f */
		b += 32 * x1;	/* 0.1255f */

		x1 = c1m1y * k;		/* 0 0 1 1 */
		x0 = (c1m1y<<8) - x1;	/* 0 0 1 0 */
		x1 >>= 8;		/* From 23 fractional bits to 15 */
		r += 28 * x1;	/* 0.1098f */
		g += 26 * x1;	/* 0.1020f */
		r += x0;
		x0 >>= 8;		/* From 23 fractional bits to 15 */
		g += 243 * x0;	/* 0.9490f */

		x1 = c1my1 * k;		/* 0 1 0 1 */
		x0 = (c1my1<<8) - x1;	/* 0 1 0 0 */
		x1 >>= 8;		/* From 23 fractional bits to 15 */
		x0 >>= 8;		/* From 23 fractional bits to 15 */
		r += 36 * x1;	/* 0.1412f */
		r += 237 * x0;	/* 0.9255f */
		b += 141 * x0;	/* 0.5490f */

		x1 = c1my * k;		/* 0 1 1 1 */
		x0 = (c1my<<8) - x1;	/* 0 1 1 0 */
		x1 >>= 8;		/* From 23 fractional bits to 15 */
		x0 >>= 8;		/* From 23 fractional bits to 15 */
		r += 34 * x1;	/* 0.1333f */
		r += 238 * x0;	/* 0.9294f */
		g += 28 * x0;	/* 0.1098f */
		b += 36 * x0;	/* 0.1412f */

		x1 = cm1y1 * k;		/* 1 0 0 1 */
		x0 = (cm1y1<<8) - x1;	/* 1 0 0 0 */
		x1 >>= 8;		/* From 23 fractional bits to 15 */
		x0 >>= 8;		/* From 23 fractional bits to 15 */
		g += 15 * x1;	/* 0.0588f */
		b += 36 * x1;	/* 0.1412f */
		g += 174 * x0;	/* 0.6784f */
		b += 240 * x0;	/* 0.9373f */

		x1 = cm1y * k;		/* 1 0 1 1 */
		x0 = (cm1y<<8) - x1;	/* 1 0 1 0 */
		x1 >>= 8;		/* From 23 fractional bits to 15 */
		x0 >>= 8;		/* From 23 fractional bits to 15 */
		g += 19 * x1;	/* 0.0745f */
		g += 167 * x0;	/* 0.6510f */
		b += 80 * x0;	/* 0.3137f */

		x1 = cmy1 * k;		/* 1 1 0 1 */
		x0 = (cmy1<<8) - x1;	/* 1 1 0 0 */
		x1 >>= 8;		/* From 23 fractional bits to 15 */
		x0 >>= 8;		/* From 23 fractional bits to 15 */
		b += 2 * x1;	/* 0.0078f */
		r += 46 * x0;	/* 0.1804f */
		g += 49 * x0;	/* 0.1922f */
		b += 147 * x0;	/* 0.5725f */

		x0 = cmy * (256-k);	/* 1 1 1 0 */
		x0 >>= 8;		/* From 23 fractional bits to 15 */
		r += 54 * x0;	/* 0.2118f */
		g += 54 * x0;	/* 0.2119f */
		b += 57 * x0;	/* 0.2235f */

		r -= (r>>8);
		g -= (g>>8);
		b -= (b>>8);
		*pr = r>>23;
		*pg = g>>23;
		*pb = b>>23;
	}
#else
	*pr = 255 - (unsigned char)fz_mini(c + k, 255);
	*pg = 255 - (unsigned char)fz_mini(m + k, 255);
	*pb = 255 - (unsigned char)fz_mini(y + k, 255);
#endif
}