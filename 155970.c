lzh_read_pt_bitlen(struct lzh_stream *strm, int start, int end)
{
	struct lzh_dec *ds = strm->ds;
	struct lzh_br *br = &(ds->br);
	int c, i;

	for (i = start; i < end; ) {
		/*
		 *  bit pattern     the number we need
		 *     000           ->  0
		 *     001           ->  1
		 *     010           ->  2
		 *     ...
		 *     110           ->  6
		 *     1110          ->  7
		 *     11110         ->  8
		 *     ...
		 *     1111111111110 ->  16
		 */
		if (!lzh_br_read_ahead(strm, br, 3))
			return (i);
		if ((c = lzh_br_bits(br, 3)) == 7) {
			if (!lzh_br_read_ahead(strm, br, 13))
				return (i);
			c = bitlen_tbl[lzh_br_bits(br, 13) & 0x3FF];
			if (c)
				lzh_br_consume(br, c - 3);
			else
				return (-1);/* Invalid data. */
		} else
			lzh_br_consume(br, 3);
		ds->pt.bitlen[i++] = c;
		ds->pt.freq[c]++;
	}
	return (i);
}