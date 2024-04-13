fill_label_bitfield_varint64(field_info *fi, gchar *label_str, gboolean is_signed)
{
	char       *p;
	int         bitfield_byte_length;
	guint64     unshifted_value;
	guint64     value;

	char        buf[48];
	const char *out;

	header_field_info *hfinfo = fi->hfinfo;

	/* Un-shift bits */
	if (is_signed) {
		value = fvalue_get_sinteger64(&fi->value);
	} else {
		value = fvalue_get_uinteger64(&fi->value);
	}
	unshifted_value = value;
	if (hfinfo->bitmask) {
		unshifted_value <<= hfinfo_bitshift(hfinfo);
	}

	/* Create the bitfield first */
	p = decode_bitfield_varint_value(label_str, unshifted_value, hfinfo->bitmask, fi->length*8);
	bitfield_byte_length = (int) (p - label_str);

	/* Fill in the textual info using stored (shifted) value */
	if (hfinfo->display == BASE_CUSTOM) {
		gchar tmp[ITEM_LABEL_LENGTH];
		const custom_fmt_func_64_t fmtfunc64 = (const custom_fmt_func_64_t)hfinfo->strings;

		DISSECTOR_ASSERT(fmtfunc64);
		fmtfunc64(tmp, value);
		label_fill(label_str, bitfield_byte_length, hfinfo, tmp);
	}
	else if (hfinfo->strings) {
		const char *val_str = hf_try_val64_to_str_const(value, hfinfo, "Unknown");

		out = hfinfo_number_vals_format64(hfinfo, buf, value);
		if (out == NULL) /* BASE_NONE so don't put integer in descr */
			label_fill(label_str, bitfield_byte_length, hfinfo, val_str);
		else
			label_fill_descr(label_str, bitfield_byte_length, hfinfo, val_str, out);
	}
	else {
		out = hfinfo_number_value_format64(hfinfo, buf, value);

		label_fill(label_str, bitfield_byte_length, hfinfo, out);
	}
}