finfo_set_len(field_info *fi, const gint length)
{
	gint length_remaining;

	DISSECTOR_ASSERT_HINT(length >= 0, fi->hfinfo->abbrev);
	length_remaining = tvb_captured_length_remaining(fi->ds_tvb, fi->start);
	if (length > length_remaining)
		fi->length = length_remaining;
	else
		fi->length = length;

	/*
	 * You cannot just make the "len" field of a GByteArray
	 * larger, if there's no data to back that length;
	 * you can only make it smaller.
	 */
	if (fi->value.ftype->ftype == FT_BYTES && fi->length <= (gint)fi->value.value.bytes->len)
		fi->value.value.bytes->len = fi->length;
}