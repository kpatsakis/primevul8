free_GPtrArray_value(gpointer key, gpointer value, gpointer user_data _U_)
{
	GPtrArray         *ptrs = (GPtrArray *)value;
	gint               hfid = GPOINTER_TO_UINT(key);
	header_field_info *hfinfo;

	PROTO_REGISTRAR_GET_NTH(hfid, hfinfo);
	if (hfinfo->ref_type != HF_REF_TYPE_NONE) {
		/* when a field is referenced by a filter this also
		   affects the refcount for the parent protocol so we need
		   to adjust the refcount for the parent as well
		*/
		if (hfinfo->parent != -1) {
			header_field_info *parent_hfinfo;
			PROTO_REGISTRAR_GET_NTH(hfinfo->parent, parent_hfinfo);
			parent_hfinfo->ref_type = HF_REF_TYPE_NONE;
		}
		hfinfo->ref_type = HF_REF_TYPE_NONE;
	}

	g_ptr_array_free(ptrs, TRUE);
}