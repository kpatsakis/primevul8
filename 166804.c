detect_trailing_stray_characters(guint encoding, const char *string, gint length, proto_item *pi)
{
	gboolean found_stray_character = FALSE;

	if (!string)
		return;

	switch (encoding & ENC_CHARENCODING_MASK) {
		case ENC_ASCII:
		case ENC_UTF_8:
			for (gint i = (gint)strlen(string); i < length; i++) {
				if (string[i] != '\0') {
					found_stray_character = TRUE;
					break;
				}
			}
			break;

		default:
			break;
	}

	if (found_stray_character) {
		expert_add_info(NULL, pi, &ei_string_trailing_characters);
	}
}