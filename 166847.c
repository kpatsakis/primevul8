proto_item_set_end(proto_item *pi, tvbuff_t *tvb, gint end)
{
	field_info *fi;
	gint length;

	TRY_TO_FAKE_THIS_REPR_VOID(pi);

	fi = PITEM_FINFO(pi);
	if (fi == NULL)
		return;

	end += tvb_raw_offset(tvb);
	DISSECTOR_ASSERT(end >= fi->start);
	length = end - fi->start;

	finfo_set_len(fi, length);
}