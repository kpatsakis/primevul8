hfinfo_remove_from_gpa_name_map(const header_field_info *hfinfo)
{
	g_free(last_field_name);
	last_field_name = NULL;

	if (!hfinfo->same_name_next && hfinfo->same_name_prev_id == -1) {
		/* No hfinfo with the same name */
		g_hash_table_steal(gpa_name_map, hfinfo->abbrev);
		return;
	}

	if (hfinfo->same_name_next) {
		hfinfo->same_name_next->same_name_prev_id = hfinfo->same_name_prev_id;
	}

	if (hfinfo->same_name_prev_id != -1) {
		header_field_info *same_name_prev = hfinfo_same_name_get_prev(hfinfo);
		same_name_prev->same_name_next = hfinfo->same_name_next;
		if (!hfinfo->same_name_next) {
			/* It's always the latest added hfinfo which is stored in gpa_name_map */
			g_hash_table_insert(gpa_name_map, (gpointer) (same_name_prev->abbrev), same_name_prev);
		}
	}
}