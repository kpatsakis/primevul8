proto_item_set_len(proto_item *pi, const gint length)
{
	field_info *fi;

	TRY_TO_FAKE_THIS_REPR_VOID(pi);

	fi = PITEM_FINFO(pi);
	if (fi == NULL)
		return;

	finfo_set_len(fi, length);
}