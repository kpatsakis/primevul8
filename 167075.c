hfinfo_number_value_format_display64(const header_field_info *hfinfo, int display, char buf[48], guint64 value)
{
	char *ptr = &buf[47];
	gboolean isint = IS_FT_INT(hfinfo->type);

	*ptr = '\0';
	/* Properly format value */
	switch (FIELD_DISPLAY(display)) {
		case BASE_DEC:
			return isint ? int64_to_str_back(ptr, (gint64) value) : uint64_to_str_back(ptr, value);

		case BASE_DEC_HEX:
			*(--ptr) = ')';
			ptr = hex64_to_str_back(ptr, hfinfo_hex_digits(hfinfo), value);
			*(--ptr) = '(';
			*(--ptr) = ' ';
			ptr = isint ? int64_to_str_back(ptr, (gint64) value) : uint64_to_str_back(ptr, value);
			return ptr;

		case BASE_OCT:
			return oct64_to_str_back(ptr, value);

		case BASE_HEX:
			return hex64_to_str_back(ptr, hfinfo_hex_digits(hfinfo), value);

		case BASE_HEX_DEC:
			*(--ptr) = ')';
			ptr = isint ? int64_to_str_back(ptr, (gint64) value) : uint64_to_str_back(ptr, value);
			*(--ptr) = '(';
			*(--ptr) = ' ';
			ptr = hex64_to_str_back(ptr, hfinfo_hex_digits(hfinfo), value);
			return ptr;

		default:
			g_assert_not_reached();
	}

	return ptr;
}