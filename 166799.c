hfinfo_numeric_value_format64(const header_field_info *hfinfo, char buf[64], guint64 value)
{
	/* Get the underlying BASE_ value */
	int display = FIELD_DISPLAY(hfinfo->display);

	if (hfinfo->type == FT_FRAMENUM) {
		/*
		 * Frame numbers are always displayed in decimal.
		 */
		display = BASE_DEC;
	}

	switch (display) {
		case BASE_NONE:
		/* case BASE_DEC: */
		case BASE_DEC_HEX:
		case BASE_OCT: /* XXX, why we're changing BASE_OCT to BASE_DEC? */
		case BASE_CUSTOM:
			display = BASE_DEC;
			break;

		/* case BASE_HEX: */
		case BASE_HEX_DEC:
			display = BASE_HEX;
			break;
	}

	return hfinfo_number_value_format_display64(hfinfo, display, buf, value);
}