label_mark_truncated(char *label_str, gsize name_pos)
{
	static const char  trunc_str[] = " [truncated]";
	const size_t       trunc_len = sizeof(trunc_str)-1;
	gchar             *last_char;

	/* ..... field_name: dataaaaaaaaaaaaa
	 *                 |
	 *                 ^^^^^ name_pos
	 *
	 * ..... field_name [truncated]: dataaaaaaaaaaaaa
	 *
	 * name_pos==0 means that we have only data or only a field_name
	 */

	if (name_pos < ITEM_LABEL_LENGTH - trunc_len) {
		memmove(label_str + name_pos + trunc_len, label_str + name_pos, ITEM_LABEL_LENGTH - name_pos - trunc_len);
		memcpy(label_str + name_pos, trunc_str, trunc_len);

		/* in general, label_str is UTF-8
		   we can truncate it only at the beginning of a new character
		   we go backwards from the byte right after our buffer and
		    find the next starting byte of a UTF-8 character, this is
		    where we cut
		   there's no need to use g_utf8_find_prev_char(), the search
		    will always succeed since we copied trunc_str into the
		    buffer */
		last_char = g_utf8_prev_char(&label_str[ITEM_LABEL_LENGTH - 1]);
		*last_char = '\0';

	} else if (name_pos < ITEM_LABEL_LENGTH)
		g_strlcpy(label_str + name_pos, trunc_str, ITEM_LABEL_LENGTH - name_pos);
}