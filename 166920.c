proto_can_match_selected(field_info *finfo, epan_dissect_t *edt)
{
	return construct_match_selected_string(finfo, edt, NULL);
}