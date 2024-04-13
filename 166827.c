free_deregistered_field (gpointer data, gpointer user_data _U_)
{
	header_field_info *hfi = (header_field_info *) data;
	gint hf_id = hfi->id;

	g_free((char *)hfi->name);
	g_free((char *)hfi->abbrev);
	g_free((char *)hfi->blurb);

	proto_free_field_strings(hfi->type, hfi->display, hfi->strings);

	if (hfi->parent == -1)
		g_slice_free(header_field_info, hfi);

	gpa_hfinfo.hfi[hf_id] = NULL; /* Invalidate this hf_id / proto_id */
}