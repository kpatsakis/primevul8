get_stringz_value(wmem_allocator_t *scope, proto_tree *tree, tvbuff_t *tvb,
    gint start, gint length, gint *ret_length, const guint encoding)
{
	const guint8 *value;

	if (length < -1) {
		report_type_length_mismatch(tree, "a string", length, TRUE);
	}
	if (length == -1) {
		/* This can throw an exception */
		value = tvb_get_stringz_enc(scope, tvb, start, &length, encoding);
	} else {
		/* In this case, length signifies the length of the string.
		 *
		 * This could either be a null-padded string, which doesn't
		 * necessarily have a '\0' at the end, or a null-terminated
		 * string, with a trailing '\0'.  (Yes, there are cases
		 * where you have a string that's both counted and null-
		 * terminated.)
		 *
		 * In the first case, we must allocate a buffer of length
		 * "length+1", to make room for a trailing '\0'.
		 *
		 * In the second case, we don't assume that there is a
		 * trailing '\0' there, as the packet might be malformed.
		 * (XXX - should we throw an exception if there's no
		 * trailing '\0'?)  Therefore, we allocate a buffer of
		 * length "length+1", and put in a trailing '\0', just to
		 * be safe.
		 *
		 * (XXX - this would change if we made string values counted
		 * rather than null-terminated.)
		 */
		value = tvb_get_string_enc(scope, tvb, start, length, encoding);
	}
	*ret_length = length;
	return value;
}