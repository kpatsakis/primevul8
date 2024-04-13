hfinfo_numeric_value_format(const header_field_info *hfinfo, char buf[32], guint32 value)
{
	/* Get the underlying BASE_ value */
	int display = FIELD_DISPLAY(hfinfo->display);

	if (hfinfo->type == FT_FRAMENUM) {
		/*
		 * Frame numbers are always displayed in decimal.
		 */
		display = BASE_DEC;
	}

	if (IS_BASE_PORT(display)) {
		display = BASE_DEC;
	} else if (display == BASE_OUI) {
		display = BASE_HEX;
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

	return hfinfo_number_value_format_display(hfinfo, display, buf, value);
}