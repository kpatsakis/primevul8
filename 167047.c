find_protocol_by_id(const int proto_id)
{
	header_field_info *hfinfo;

	if (proto_id < 0)
		return NULL;

	PROTO_REGISTRAR_GET_NTH(proto_id, hfinfo);
	if (hfinfo->type != FT_PROTOCOL) {
		DISSECTOR_ASSERT(hfinfo->display & BASE_PROTOCOL_INFO);
	}
	return (protocol_t *)hfinfo->strings;
}