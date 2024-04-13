proto_construct_match_selected_string(field_info *finfo, epan_dissect_t *edt)
{
	char *filter = NULL;

	if (!construct_match_selected_string(finfo, edt, &filter))
	{
		wmem_free(NULL, filter);
		return NULL;
	}
	return filter;
}