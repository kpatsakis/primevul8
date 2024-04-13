proto_item_prepend_text(proto_item *pi, const char *format, ...)
{
	field_info *fi = NULL;
	char        representation[ITEM_LABEL_LENGTH];
	va_list     ap;

	TRY_TO_FAKE_THIS_REPR_VOID(pi);

	fi = PITEM_FINFO(pi);
	if (fi == NULL) {
		return;
	}

	if (!proto_item_is_hidden(pi)) {
		/*
		 * If we don't already have a representation,
		 * generate the default representation.
		 */
		if (fi->rep == NULL) {
			ITEM_LABEL_NEW(PNODE_POOL(pi), fi->rep);
			proto_item_fill_label(fi, representation);
		} else
			g_strlcpy(representation, fi->rep->representation, ITEM_LABEL_LENGTH);

		va_start(ap, format);
		g_vsnprintf(fi->rep->representation,
			ITEM_LABEL_LENGTH, format, ap);
		va_end(ap);
		g_strlcat(fi->rep->representation, representation, ITEM_LABEL_LENGTH);
	}
}