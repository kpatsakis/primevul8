void http_check_request_for_cacheability(struct stream *s, struct channel *req)
{
	struct http_txn *txn = s->txn;
	struct htx *htx;
	struct http_hdr_ctx ctx = { .blk = NULL };
	int pragma_found, cc_found;

	if ((txn->flags & (TX_CACHEABLE|TX_CACHE_IGNORE)) == TX_CACHE_IGNORE)
		return; /* nothing more to do here */

	htx = htxbuf(&req->buf);
	pragma_found = cc_found = 0;

	/* Check "pragma" header for HTTP/1.0 compatibility. */
	if (http_find_header(htx, ist("pragma"), &ctx, 1)) {
		if (isteqi(ctx.value, ist("no-cache"))) {
			pragma_found = 1;
		}
	}

	ctx.blk = NULL;
	/* Don't use the cache and don't try to store if we found the
	 * Authorization header */
	if (http_find_header(htx, ist("authorization"), &ctx, 1)) {
		txn->flags &= ~TX_CACHEABLE & ~TX_CACHE_COOK;
		txn->flags |= TX_CACHE_IGNORE;
	}


	/* Look for "cache-control" header and iterate over all the values
	 * until we find one that specifies that caching is possible or not. */
	ctx.blk = NULL;
	while (http_find_header(htx, ist("cache-control"), &ctx, 0)) {
		cc_found = 1;
		/* We don't check the values after max-age, max-stale nor min-fresh,
		 * we simply don't use the cache when they're specified. */
		if (istmatchi(ctx.value, ist("max-age")) ||
		    istmatchi(ctx.value, ist("no-cache")) ||
		    istmatchi(ctx.value, ist("max-stale")) ||
		    istmatchi(ctx.value, ist("min-fresh"))) {
			txn->flags |= TX_CACHE_IGNORE;
			continue;
		}
		if (istmatchi(ctx.value, ist("no-store"))) {
			txn->flags &= ~TX_CACHEABLE & ~TX_CACHE_COOK;
			continue;
		}
	}

	/* RFC7234#5.4:
	 *   When the Cache-Control header field is also present and
	 *   understood in a request, Pragma is ignored.
	 *   When the Cache-Control header field is not present in a
	 *   request, caches MUST consider the no-cache request
	 *   pragma-directive as having the same effect as if
	 *   "Cache-Control: no-cache" were present.
	 */
	if (!cc_found && pragma_found)
		txn->flags |= TX_CACHE_IGNORE;
}