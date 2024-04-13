static char *str_dup_safe(const ut8 *b, const ut8 *str, const ut8 *end) {
	if (str >= b && str < end) {
		int len = r_str_nlen ((const char *)str, end - str);
		if (len) {
			return r_str_ndup ((const char *)str, len);
		}
	}
	return NULL;
}