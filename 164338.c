collector_decode_htmlnumericentity(int c, void *data)
{
	struct collector_htmlnumericentity_data *pc = (struct collector_htmlnumericentity_data *)data;
	int f, n, s, r, d, size, *mapelm;

	switch (pc->status) {
	case 1:
		if (c == 0x23) {	/* '#' */
			pc->status = 2;
		} else {
			pc->status = 0;
			(*pc->decoder->filter_function)(0x26, pc->decoder);		/* '&' */
			(*pc->decoder->filter_function)(c, pc->decoder);
		}
		break;
	case 2:
		if (c == 0x78) {	/* 'x' */
			pc->status = 4;
		} else if (c >= 0x30 && c <= 0x39) { /* '0' - '9' */
			pc->cache = c - 0x30;
			pc->status = 3;
			pc->digit = 1;
		} else {
			pc->status = 0;
			(*pc->decoder->filter_function)(0x26, pc->decoder);		/* '&' */
			(*pc->decoder->filter_function)(0x23, pc->decoder);		/* '#' */
			(*pc->decoder->filter_function)(c, pc->decoder);
		}
		break;
	case 3:
		s = 0;
		f = 0;
		if (c >= 0x30 && c <= 0x39) {	/* '0' - '9' */
			if (pc->digit > 9) {
				pc->status = 0;
				s = pc->cache;
				f = 1;
			} else {
				s = pc->cache*10 + c - 0x30;
				pc->cache = s;
				pc->digit++;
			}
		} else {
			pc->status = 0;
			s = pc->cache;
			f = 1;
			n = 0;
			size = pc->mapsize;
			while (n < size) {
				mapelm = &(pc->convmap[n*4]);
				d = s - mapelm[2];
				if (d >= mapelm[0] && d <= mapelm[1]) {
					f = 0;
					(*pc->decoder->filter_function)(d, pc->decoder);
					if (c != 0x3b) {	/* ';' */
						(*pc->decoder->filter_function)(c, pc->decoder);
					}
					break;
				}
				n++;
			}
		}
		if (f) {
			(*pc->decoder->filter_function)(0x26, pc->decoder);		/* '&' */
			(*pc->decoder->filter_function)(0x23, pc->decoder);		/* '#' */
			r = 1;
			n = pc->digit;
			while (n > 0) {
				r *= 10;
				n--;
			}
			s %= r;
			r /= 10;
			while (r > 0) {
				d = s/r;
				s %= r;
				r /= 10;
				(*pc->decoder->filter_function)(mbfl_hexchar_table[d], pc->decoder);
			}
			(*pc->decoder->filter_function)(c, pc->decoder);
		}
		break;
	case 4:
		if (c >= 0x30 && c <= 0x39) { /* '0' - '9' */
			pc->cache = c - 0x30;
			pc->status = 5;
			pc->digit = 1;
		} else if (c >= 0x41 && c <= 0x46) { /* 'A' - 'F'  */
			pc->cache = c - 0x41 + 10;
			pc->status = 5;
			pc->digit = 1;
		} else if (c >= 0x61 && c <= 0x66) { /* 'a' - 'f'  */
			pc->cache = c - 0x61 + 10;
			pc->status = 5;
			pc->digit = 1;
		} else {
			pc->status = 0;
			(*pc->decoder->filter_function)(0x26, pc->decoder);		/* '&' */
			(*pc->decoder->filter_function)(0x23, pc->decoder);		/* '#' */
			(*pc->decoder->filter_function)(0x78, pc->decoder);		/* 'x' */
			(*pc->decoder->filter_function)(c, pc->decoder);
		}
		break;
	case 5:
		s = 0;
		f = 0;
		if ((c >= 0x30 && c <= 0x39) ||
			(c >= 0x41 && c <= 0x46) ||
			(c >= 0x61 && c <= 0x66)) {	/* '0' - '9' or 'a' - 'f'  */
			if (pc->digit > 9) {
				pc->status = 0;
				s = pc->cache;
				f = 1;
			} else {
				if (c >= 0x30 && c <= 0x39) {
					s = pc->cache*16 + (c - 0x30);
				} else if (c >= 0x41 && c <= 0x46)  {
					s = pc->cache*16 + (c - 0x41 + 10);
				} else {
					s = pc->cache*16 + (c - 0x61 + 10);
				}
				pc->cache = s;
				pc->digit++;
			}
		} else {
			pc->status = 0;
			s = pc->cache;
			f = 1;
			n = 0;
			size = pc->mapsize;
			while (n < size) {
				mapelm = &(pc->convmap[n*4]);
				d = s - mapelm[2];
				if (d >= mapelm[0] && d <= mapelm[1]) {
					f = 0;
					(*pc->decoder->filter_function)(d, pc->decoder);
					if (c != 0x3b) {	/* ';' */
						(*pc->decoder->filter_function)(c, pc->decoder);
					}
					break;
				}
				n++;
			}
		}
		if (f) {
			(*pc->decoder->filter_function)(0x26, pc->decoder);		/* '&' */
			(*pc->decoder->filter_function)(0x23, pc->decoder);		/* '#' */
			(*pc->decoder->filter_function)(0x78, pc->decoder);		/* 'x' */
			r = 1;
			n = pc->digit;
			while (n > 0) {
				r *= 16;
				n--;
			}
			s %= r;
			r /= 16;
			while (r > 0) {
				d = s/r;
				s %= r;
				r /= 16;
				(*pc->decoder->filter_function)(mbfl_hexchar_table[d], pc->decoder);
			}
			(*pc->decoder->filter_function)(c, pc->decoder);
		}
		break;
	default:
		if (c == 0x26) {	/* '&' */
			pc->status = 1;
		} else {
			(*pc->decoder->filter_function)(c, pc->decoder);
		}
		break;
	}

	return c;
}