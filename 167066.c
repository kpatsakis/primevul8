proto_field_is_referenced(proto_tree *tree, int proto_id)
{
	register header_field_info *hfinfo;


	if (!tree)
		return FALSE;

	if (PTREE_DATA(tree)->visible)
		return TRUE;

	PROTO_REGISTRAR_GET_NTH(proto_id, hfinfo);
	if (hfinfo->ref_type != HF_REF_TYPE_NONE)
		return TRUE;

	if (hfinfo->type == FT_PROTOCOL && !PTREE_DATA(tree)->fake_protocols)
		return TRUE;

	return FALSE;
}