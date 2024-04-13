static http_meth_t find_http_meth(const char *str, const int len)
{
	unsigned char m;
	const struct http_method_desc *h;

	m = ((unsigned)*str - 'A');

	if (m < 26) {
		for (h = http_methods[m]; h->len > 0; h++) {
			if (unlikely(h->len != len))
				continue;
			if (likely(memcmp(str, h->text, h->len) == 0))
				return h->meth;
		};
		return HTTP_METH_OTHER;
	}
	return HTTP_METH_NONE;

}