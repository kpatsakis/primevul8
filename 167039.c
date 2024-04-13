fill_label_bitfield_char(field_info *fi, gchar *label_str)
{
	char       *p;
	int         bitfield_byte_length, bitwidth;
	guint32     unshifted_value;
	guint32     value;

	char        buf[32];
	const char *out;

	header_field_info *hfinfo = fi->hfinfo;

	/* Figure out the bit width */
	bitwidth = hfinfo_container_bitwidth(hfinfo);

	/* Un-shift bits */
	value = fvalue_get_uinteger(&fi->value);

	unshifted_value = value;
	if (hfinfo->bitmask) {
		unshifted_value <<= hfinfo_bitshift(hfinfo);
	}

	/* Create the bitfield first */
	p = decode_bitfield_value(label_str, unshifted_value, hfinfo->bitmask, bitwidth);
	bitfield_byte_length = (int) (p - label_str);

	/* Fill in the textual info using stored (shifted) value */
	if (hfinfo->display == BASE_CUSTOM) {
		gchar tmp[ITEM_LABEL_LENGTH];
		const custom_fmt_func_t fmtfunc = (const custom_fmt_func_t)hfinfo->strings;

		DISSECTOR_ASSERT(fmtfunc);
		fmtfunc(tmp, value);
		label_fill(label_str, bitfield_byte_length, hfinfo, tmp);
	}
	else if (hfinfo->strings) {
		const char *val_str = hf_try_val_to_str_const(value, hfinfo, "Unknown");

		out = hfinfo_char_vals_format(hfinfo, buf, value);
		if (out == NULL) /* BASE_NONE so don't put integer in descr */
			label_fill(label_str, bitfield_byte_length, hfinfo, val_str);
		else
			label_fill_descr(label_str, bitfield_byte_length, hfinfo, val_str, out);
	}
	else {
		out = hfinfo_char_value_format(hfinfo, buf, value);

		label_fill(label_str, bitfield_byte_length, hfinfo, out);
	}
}