get_stringztrunc_value(wmem_allocator_t *scope, tvbuff_t *tvb, gint start,
    gint length, gint *ret_length, const guint encoding)
{
	/*
	 * XXX - currently, string values are null-
	 * terminated, so a "zero-truncated" string
	 * isn't special.  If we represent string
	 * values as something that includes a counted
	 * array of bytes, we'll need to strip everything
	 * starting with the terminating NUL.
	 */
	if (length == -1) {
		length = tvb_ensure_captured_length_remaining(tvb, start);
	}
	*ret_length = length;
	return tvb_get_string_enc(scope, tvb, start, length, encoding);
}