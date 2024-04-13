hfinfo_number_value_format(const header_field_info *hfinfo, char buf[32], guint32 value)
{
	int display = hfinfo->display;

	if (hfinfo->type == FT_FRAMENUM) {
		/*
		 * Frame numbers are always displayed in decimal.
		 */
		display = BASE_DEC;
	}

	return hfinfo_number_value_format_display(hfinfo, display, buf, value);
}