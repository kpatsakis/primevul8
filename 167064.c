proto_registrar_get_id_byname(const char *field_name)
{
	header_field_info *hfinfo;

	hfinfo = proto_registrar_get_byname(field_name);

	if (!hfinfo)
		return -1;

	return hfinfo->id;
}