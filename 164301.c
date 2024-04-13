int mbfl_filt_decode_htmlnumericentity_flush(mbfl_convert_filter *filter)
{
	struct collector_htmlnumericentity_data *pc = (struct collector_htmlnumericentity_data *)filter;
	int n, s, r, d;

	if (pc->status) {
		switch (pc->status) {
		case 1: /* '&' */
			(*pc->decoder->filter_function)(0x26, pc->decoder);		/* '&' */
			break;
		case 2: /* '#' */
			(*pc->decoder->filter_function)(0x26, pc->decoder);		/* '&' */
			(*pc->decoder->filter_function)(0x23, pc->decoder);		/* '#' */
			break;
		case 3: /* '0'-'9' */
			(*pc->decoder->filter_function)(0x26, pc->decoder);		/* '&' */
			(*pc->decoder->filter_function)(0x23, pc->decoder);		/* '#' */

			s = pc->cache;
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

			break;
		case 4: /* 'x' */
			(*pc->decoder->filter_function)(0x26, pc->decoder);		/* '&' */
			(*pc->decoder->filter_function)(0x23, pc->decoder);		/* '#' */
			(*pc->decoder->filter_function)(0x78, pc->decoder);		/* 'x' */
			break;
		case 5: /* '0'-'9','a'-'f' */
			(*pc->decoder->filter_function)(0x26, pc->decoder);		/* '&' */
			(*pc->decoder->filter_function)(0x23, pc->decoder);		/* '#' */
			(*pc->decoder->filter_function)(0x78, pc->decoder);		/* 'x' */

			s = pc->cache;
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
			break;
		default:
			break;
		}
	}

	pc->status = 0;
	pc->cache = 0;
	pc->digit = 0;

	return 0;
}