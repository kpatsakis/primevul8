proto_item_append_text(proto_item *pi, const char *format, ...)
{
	field_info *fi = NULL;
	size_t      curlen;
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
			proto_item_fill_label(fi, fi->rep->representation);
		}

		curlen = strlen(fi->rep->representation);
		if (ITEM_LABEL_LENGTH > curlen) {
			va_start(ap, format);
			g_vsnprintf(fi->rep->representation + curlen,
				ITEM_LABEL_LENGTH - (gulong) curlen, format, ap);
			va_end(ap);
		}
	}
}