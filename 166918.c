check_valid_filter_name_or_fail(const char *filter_name)
{
	gboolean found_invalid = proto_check_field_name(filter_name);

	/* Additionally forbid upper case characters. */
	if (!found_invalid) {
		for (guint i = 0; filter_name[i]; i++) {
			if (g_ascii_isupper(filter_name[i])) {
				found_invalid = TRUE;
				break;
			}
		}
	}

	if (found_invalid) {
		g_error("Protocol filter name \"%s\" has one or more invalid characters."
			" Allowed are lower characters, digits, '-', '_' and non-repeating '.'."
			" This might be caused by an inappropriate plugin or a development error.", filter_name);
	}
}