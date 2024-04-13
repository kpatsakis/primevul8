fill_label_char(field_info *fi, gchar *label_str)
{
	header_field_info *hfinfo = fi->hfinfo;
	guint32            value;

	char               buf[32];
	const char        *out;

	value = fvalue_get_uinteger(&fi->value);

	/* Fill in the textual info */
	if (hfinfo->display == BASE_CUSTOM) {
		gchar tmp[ITEM_LABEL_LENGTH];
		const custom_fmt_func_t fmtfunc = (const custom_fmt_func_t)hfinfo->strings;

		DISSECTOR_ASSERT(fmtfunc);
		fmtfunc(tmp, value);
		label_fill(label_str, 0, hfinfo, tmp);
	}
	else if (hfinfo->strings) {
		const char *val_str = hf_try_val_to_str_const(value, hfinfo, "Unknown");

		out = hfinfo_char_vals_format(hfinfo, buf, value);
		label_fill_descr(label_str, 0, hfinfo, val_str, out);
	}
	else {
		out = hfinfo_char_value_format(hfinfo, buf, value);

		label_fill(label_str, 0, hfinfo, out);
	}
}