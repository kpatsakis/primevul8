proto_item_get_len(const proto_item *pi)
{
	field_info *fi;

	if (!pi)
		return -1;
	fi = PITEM_FINFO(pi);
	return fi ? fi->length : -1;
}