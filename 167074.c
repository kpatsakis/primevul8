fill_label_number(field_info *fi, gchar *label_str, gboolean is_signed)
{
	header_field_info *hfinfo = fi->hfinfo;
	guint32            value;

	char               buf[32];
	const char        *out;

	if (is_signed)
		value = fvalue_get_sinteger(&fi->value);
	else
		value = fvalue_get_uinteger(&fi->value);

	/* Fill in the textual info */
	if (hfinfo->display == BASE_CUSTOM) {
		gchar tmp[ITEM_LABEL_LENGTH];
		const custom_fmt_func_t fmtfunc = (const custom_fmt_func_t)hfinfo->strings;

		DISSECTOR_ASSERT(fmtfunc);
		fmtfunc(tmp, value);
		label_fill(label_str, 0, hfinfo, tmp);
	}
	else if (hfinfo->strings && hfinfo->type != FT_FRAMENUM) {
		/*
		 * It makes no sense to have a value-string table for a
		 * frame-number field - they're just integers giving
		 * the ordinal frame number.
		 */
		const char *val_str = hf_try_val_to_str(value, hfinfo);

		out = hfinfo_number_vals_format(hfinfo, buf, value);
		if (hfinfo->display & BASE_SPECIAL_VALS) {
			/*
			 * Unique values only display value_string string
			 * if there is a match. Otherwise it's just a number
			 */
			if (val_str) {
				label_fill_descr(label_str, 0, hfinfo, val_str, out);
			} else {
				label_fill(label_str, 0, hfinfo, out);
			}
		} else {
			if (val_str == NULL)
				val_str = "Unknown";

			if (out == NULL) /* BASE_NONE so don't put integer in descr */
				label_fill(label_str, 0, hfinfo, val_str);
			else
				label_fill_descr(label_str, 0, hfinfo, val_str, out);
		}
	}
	else if (IS_BASE_PORT(hfinfo->display)) {
		gchar tmp[ITEM_LABEL_LENGTH];

		port_with_resolution_to_str_buf(tmp, sizeof(tmp),
			display_to_port_type((field_display_e)hfinfo->display), value);
		label_fill(label_str, 0, hfinfo, tmp);
	}
	else {
		out = hfinfo_number_value_format(hfinfo, buf, value);

		label_fill(label_str, 0, hfinfo, out);
	}
}