collector_encode_hex_htmlnumericentity(int c, void *data)
{
	struct collector_htmlnumericentity_data *pc = (struct collector_htmlnumericentity_data *)data;
	int f, n, s, r, d, size, *mapelm;

	size = pc->mapsize;
	f = 0;
	n = 0;
	while (n < size) {
		mapelm = &(pc->convmap[n*4]);
		if (c >= mapelm[0] && c <= mapelm[1]) {
			s = (c + mapelm[2]) & mapelm[3];
			if (s >= 0) {
				(*pc->decoder->filter_function)(0x26, pc->decoder);	/* '&' */
				(*pc->decoder->filter_function)(0x23, pc->decoder);	/* '#' */
				(*pc->decoder->filter_function)(0x78, pc->decoder);	/* 'x' */
				r = 0x1000000;
				s %= r;
				while (r > 0) {
					d = s/r;
					if (d || f) {
						f = 1;
						s %= r;
						(*pc->decoder->filter_function)(mbfl_hexchar_table[d], pc->decoder);
					}
					r /= 16;
				}
				if (!f) {
					f = 1;
					(*pc->decoder->filter_function)(mbfl_hexchar_table[0], pc->decoder);
				}
				(*pc->decoder->filter_function)(0x3b, pc->decoder);		/* ';' */
			}
		}
		if (f) {
			break;
		}
		n++;
	}
	if (!f) {
		(*pc->decoder->filter_function)(c, pc->decoder);
	}

	return c;
}