proto_item_get_display_repr(wmem_allocator_t *scope, proto_item *pi)
{
	field_info *fi;

	if (!pi)
		return "";
	fi = PITEM_FINFO(pi);
	DISSECTOR_ASSERT(fi->hfinfo != NULL);
	return fvalue_to_string_repr(scope, &fi->value, FTREPR_DISPLAY, fi->hfinfo->display);
}