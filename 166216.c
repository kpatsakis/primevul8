void http_check_response_for_cacheability(struct stream *s, struct channel *res)
{
	struct http_txn *txn = s->txn;
	struct http_hdr_ctx ctx = { .blk = NULL };
	struct htx *htx;
	int has_freshness_info = 0;
	int has_validator = 0;

	if (txn->status < 200) {
		/* do not try to cache interim responses! */
		txn->flags &= ~TX_CACHEABLE & ~TX_CACHE_COOK;
		return;
	}

	htx = htxbuf(&res->buf);
	/* Check "pragma" header for HTTP/1.0 compatibility. */
	if (http_find_header(htx, ist("pragma"), &ctx, 1)) {
		if (isteqi(ctx.value, ist("no-cache"))) {
			txn->flags &= ~TX_CACHEABLE & ~TX_CACHE_COOK;
			return;
		}
	}

	/* Look for "cache-control" header and iterate over all the values
	 * until we find one that specifies that caching is possible or not. */
	ctx.blk = NULL;
	while (http_find_header(htx, ist("cache-control"), &ctx, 0)) {
		if (isteqi(ctx.value, ist("public"))) {
			txn->flags |= TX_CACHEABLE | TX_CACHE_COOK;
			continue;
		}
		if (isteqi(ctx.value, ist("private")) ||
		    isteqi(ctx.value, ist("no-cache")) ||
		    isteqi(ctx.value, ist("no-store")) ||
		    isteqi(ctx.value, ist("max-age=0")) ||
		    isteqi(ctx.value, ist("s-maxage=0"))) {
			txn->flags &= ~TX_CACHEABLE & ~TX_CACHE_COOK;
			continue;
		}
		/* We might have a no-cache="set-cookie" form. */
		if (istmatchi(ctx.value, ist("no-cache=\"set-cookie"))) {
			txn->flags &= ~TX_CACHE_COOK;
			continue;
		}

		if (istmatchi(ctx.value, ist("s-maxage")) ||
		    istmatchi(ctx.value, ist("max-age"))) {
			has_freshness_info = 1;
			continue;
		}
	}

	/* If no freshness information could be found in Cache-Control values,
	 * look for an Expires header. */
	if (!has_freshness_info) {
		ctx.blk = NULL;
		has_freshness_info = http_find_header(htx, ist("expires"), &ctx, 0);
	}

	/* If no freshness information could be found in Cache-Control or Expires
	 * values, look for an explicit validator. */
	if (!has_freshness_info) {
		ctx.blk = NULL;
		has_validator = 1;
		if (!http_find_header(htx, ist("etag"), &ctx, 0)) {
			ctx.blk = NULL;
			if (!http_find_header(htx, ist("last-modified"), &ctx, 0))
				has_validator = 0;
		}
	}

	/* We won't store an entry that has neither a cache validator nor an
	 * explicit expiration time, as suggested in RFC 7234#3. */
	if (!has_freshness_info && !has_validator)
		txn->flags |= TX_CACHE_IGNORE;
}