proto_registrar_get_ftype(const int n)
{
	header_field_info *hfinfo;

	PROTO_REGISTRAR_GET_NTH(n, hfinfo);
	return hfinfo->type;
}