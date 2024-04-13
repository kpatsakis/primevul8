get_string_value(wmem_allocator_t *scope, tvbuff_t *tvb, gint start,
    gint length, gint *ret_length, const guint encoding)
{
	if (length == -1) {
		length = tvb_ensure_captured_length_remaining(tvb, start);
	}
	*ret_length = length;
	return tvb_get_string_enc(scope, tvb, start, length, encoding);
}