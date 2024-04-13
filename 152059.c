void php_filter_encoded(PHP_INPUT_FILTER_PARAM_DECL)
{
	/* apply strip_high and strip_low filters */
	php_filter_strip(value, flags);
	/* urlencode */
	php_filter_encode_url(value, (unsigned char *)DEFAULT_URL_ENCODE, sizeof(DEFAULT_URL_ENCODE)-1, flags & FILTER_FLAG_ENCODE_HIGH, flags & FILTER_FLAG_ENCODE_LOW, 1);
}