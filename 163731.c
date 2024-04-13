http_get_path(struct http_txn *txn)
{
	char *ptr, *end;

	ptr = txn->req.chn->buf->p + txn->req.sl.rq.u;
	end = ptr + txn->req.sl.rq.u_l;

	if (ptr >= end)
		return NULL;

	/* RFC2616, par. 5.1.2 :
	 * Request-URI = "*" | absuri | abspath | authority
	 */

	if (*ptr == '*')
		return NULL;

	if (isalpha((unsigned char)*ptr)) {
		/* this is a scheme as described by RFC3986, par. 3.1 */
		ptr++;
		while (ptr < end &&
		       (isalnum((unsigned char)*ptr) || *ptr == '+' || *ptr == '-' || *ptr == '.'))
			ptr++;
		/* skip '://' */
		if (ptr == end || *ptr++ != ':')
			return NULL;
		if (ptr == end || *ptr++ != '/')
			return NULL;
		if (ptr == end || *ptr++ != '/')
			return NULL;
	}
	/* skip [user[:passwd]@]host[:[port]] */

	while (ptr < end && *ptr != '/')
		ptr++;

	if (ptr == end)
		return NULL;

	/* OK, we got the '/' ! */
	return ptr;
}