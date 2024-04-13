fill_label_boolean(field_info *fi, gchar *label_str)
{
	char	*p                    = label_str;
	int      bitfield_byte_length = 0, bitwidth;
	guint64  unshifted_value;
	guint64  value;

	header_field_info	*hfinfo   = fi->hfinfo;
	const true_false_string	*tfstring = &tfs_true_false;

	if (hfinfo->strings) {
		tfstring = (const struct true_false_string*) hfinfo->strings;
	}

	value = fvalue_get_uinteger64(&fi->value);
	if (hfinfo->bitmask) {
		/* Figure out the bit width */
		bitwidth = hfinfo_container_bitwidth(hfinfo);

		/* Un-shift bits */
		unshifted_value = value;
		unshifted_value <<= hfinfo_bitshift(hfinfo);

		/* Create the bitfield first */
		p = decode_bitfield_value(label_str, unshifted_value, hfinfo->bitmask, bitwidth);
		bitfield_byte_length = (int) (p - label_str);
	}

	/* Fill in the textual info */
	label_fill(label_str, bitfield_byte_length, hfinfo, tfs_get_string(!!value, tfstring));
}