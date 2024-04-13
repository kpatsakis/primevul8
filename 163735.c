void capture_headers(char *som, struct hdr_idx *idx,
		     char **cap, struct cap_hdr *cap_hdr)
{
	char *eol, *sol, *col, *sov;
	int cur_idx;
	struct cap_hdr *h;
	int len;

	sol = som + hdr_idx_first_pos(idx);
	cur_idx = hdr_idx_first_idx(idx);

	while (cur_idx) {
		eol = sol + idx->v[cur_idx].len;

		col = sol;
		while (col < eol && *col != ':')
			col++;

		sov = col + 1;
		while (sov < eol && http_is_lws[(unsigned char)*sov])
			sov++;
				
		for (h = cap_hdr; h; h = h->next) {
			if ((h->namelen == col - sol) &&
			    (strncasecmp(sol, h->name, h->namelen) == 0)) {
				if (cap[h->index] == NULL)
					cap[h->index] =
						pool_alloc2(h->pool);

				if (cap[h->index] == NULL) {
					Alert("HTTP capture : out of memory.\n");
					continue;
				}
							
				len = eol - sov;
				if (len > h->len)
					len = h->len;
							
				memcpy(cap[h->index], sov, len);
				cap[h->index][len]=0;
			}
		}
		sol = eol + idx->v[cur_idx].cr + 1;
		cur_idx = idx->v[cur_idx].next;
	}
}