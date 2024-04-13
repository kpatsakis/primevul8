proto_registrar_get_nth(guint hfindex)
{
	register header_field_info *hfinfo;

	PROTO_REGISTRAR_GET_NTH(hfindex, hfinfo);
	return hfinfo;
}