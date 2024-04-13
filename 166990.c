label_fill(char *label_str, gsize pos, const header_field_info *hfinfo, const char *text)
{
	gsize name_pos;

	/* "%s: %s", hfinfo->name, text */
	name_pos = pos = label_concat(label_str, pos, hfinfo->name);
	if (!(hfinfo->display & BASE_NO_DISPLAY_VALUE)) {
		pos = label_concat(label_str, pos, ": ");
		pos = label_concat(label_str, pos, text ? text : "(null)");
	}

	if (pos >= ITEM_LABEL_LENGTH) {
		/* Uh oh, we don't have enough room. Tell the user that the field is truncated. */
		label_mark_truncated(label_str, name_pos);
	}

	return pos;
}