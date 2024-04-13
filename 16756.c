static char *str_dup_safe_fixed(const ut8 *b, const ut8 *str, ut64 len, const ut8 *end) {
	if (str >= b && str + len < end) {
		char *result = calloc (1, len + 1);
		if (result) {
			r_str_ncpy (result, (const char *)str, len);
			return result;
		}
	}
	return NULL;
}