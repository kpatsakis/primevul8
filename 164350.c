collector_strpos(int c, void* data)
{
	int *p, *h, *m, n;
	struct collector_strpos_data *pc = (struct collector_strpos_data*)data;

	if (pc->output >= pc->start) {
		if (c == (int)pc->needle.buffer[pc->needle_pos]) {
			if (pc->needle_pos == 0) {
				pc->found_pos = pc->output;			/* found position */
			}
			pc->needle_pos++;						/* needle pointer */
			if (pc->needle_pos >= pc->needle_len) {
				pc->matched_pos = pc->found_pos;	/* matched position */
				pc->needle_pos--;
				goto retry;
			}
		} else if (pc->needle_pos != 0) {
retry:
			h = (int *)pc->needle.buffer;
			h++;
			for (;;) {
				pc->found_pos++;
				p = h;
				m = (int *)pc->needle.buffer;
				n = pc->needle_pos - 1;
				while (n > 0 && *p == *m) {
					n--;
					p++;
					m++;
				}
				if (n <= 0) {
					if (*m != c) {
						pc->needle_pos = 0;
					}
					break;
				} else {
					h++;
					pc->needle_pos--;
				}
			}
		}
	}

	pc->output++;
	return c;
}