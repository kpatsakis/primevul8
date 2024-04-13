proto_registrar_is_protocol(const int n)
{
	header_field_info *hfinfo;

	PROTO_REGISTRAR_GET_NTH(n, hfinfo);
	return (((hfinfo->id != hf_text_only) && (hfinfo->parent == -1)) ? TRUE : FALSE);
}