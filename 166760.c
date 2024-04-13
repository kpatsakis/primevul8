proto_tree_set_representation(proto_item *pi, const char *format, va_list ap)
{
	int	    ret;	/*tmp return value */
	field_info *fi = PITEM_FINFO(pi);

	DISSECTOR_ASSERT(fi);

	if (!proto_item_is_hidden(pi)) {
		ITEM_LABEL_NEW(PNODE_POOL(pi), fi->rep);
		ret = g_vsnprintf(fi->rep->representation, ITEM_LABEL_LENGTH,
				  format, ap);
		if (ret >= ITEM_LABEL_LENGTH) {
			/* Uh oh, we don't have enough room.  Tell the user
			 * that the field is truncated.
			 */
			LABEL_MARK_TRUNCATED_START(fi->rep->representation);
		}
	}
}