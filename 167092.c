proto_registrar_get_length(const int n)
{
	header_field_info *hfinfo;

	PROTO_REGISTRAR_GET_NTH(n, hfinfo);
	return ftype_length(hfinfo->type);
}