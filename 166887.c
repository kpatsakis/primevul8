label_concat(char *label_str, gsize pos, const char *str)
{
	if (pos < ITEM_LABEL_LENGTH)
		pos += g_strlcpy(label_str + pos, str, ITEM_LABEL_LENGTH - pos);

	return pos;
}