proto_item_set_text(proto_item *pi, const char *format, ...)
{
	field_info *fi = NULL;
	va_list     ap;

	TRY_TO_FAKE_THIS_REPR_VOID(pi);

	fi = PITEM_FINFO(pi);
	if (fi == NULL)
		return;

	if (fi->rep) {
		ITEM_LABEL_FREE(PNODE_POOL(pi), fi->rep);
		fi->rep = NULL;
	}

	va_start(ap, format);
	proto_tree_set_representation(pi, format, ap);
	va_end(ap);
}