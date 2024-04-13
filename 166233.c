static void http_capture_headers(struct htx *htx, char **cap, struct cap_hdr *cap_hdr)
{
	struct cap_hdr *h;
	int32_t pos;

	for (pos = htx_get_first(htx); pos != -1; pos = htx_get_next(htx, pos)) {
		struct htx_blk *blk = htx_get_blk(htx, pos);
		enum htx_blk_type type = htx_get_blk_type(blk);
		struct ist n, v;

		if (type == HTX_BLK_EOH)
			break;
		if (type != HTX_BLK_HDR)
			continue;

		n = htx_get_blk_name(htx, blk);

		for (h = cap_hdr; h; h = h->next) {
			if (h->namelen && (h->namelen == n.len) &&
			    (strncasecmp(n.ptr, h->name, h->namelen) == 0)) {
				if (cap[h->index] == NULL)
					cap[h->index] =
						pool_alloc(h->pool);

				if (cap[h->index] == NULL) {
					ha_alert("HTTP capture : out of memory.\n");
					break;
				}

				v = htx_get_blk_value(htx, blk);
				v = isttrim(v, h->len);

				memcpy(cap[h->index], v.ptr, v.len);
				cap[h->index][v.len]=0;
			}
		}
	}
}