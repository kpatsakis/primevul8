proto_registrar_get_abbrev(const int n)
{
	header_field_info *hfinfo;

	PROTO_REGISTRAR_GET_NTH(n, hfinfo);
	return hfinfo->abbrev;
}