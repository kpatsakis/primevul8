hfinfo_same_name_get_prev(const header_field_info *hfinfo)
{
	header_field_info *dup_hfinfo;

	if (hfinfo->same_name_prev_id == -1)
		return NULL;
	PROTO_REGISTRAR_GET_NTH(hfinfo->same_name_prev_id, dup_hfinfo);
	return dup_hfinfo;
}