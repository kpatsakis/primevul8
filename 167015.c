hfinfo_char_vals_format(const header_field_info *hfinfo, char buf[32], guint32 value)
{
	/* Get the underlying BASE_ value */
	int display = FIELD_DISPLAY(hfinfo->display);

	return hfinfo_char_value_format_display(display, buf, value);
}