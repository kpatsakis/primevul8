proto_register_field_init(header_field_info *hfinfo, const int parent)
{

	tmp_fld_check_assert(hfinfo);

	hfinfo->parent         = parent;
	hfinfo->same_name_next = NULL;
	hfinfo->same_name_prev_id = -1;

	/* if we always add and never delete, then id == len - 1 is correct */
	if (gpa_hfinfo.len >= gpa_hfinfo.allocated_len) {
		if (!gpa_hfinfo.hfi) {
			gpa_hfinfo.allocated_len = PROTO_PRE_ALLOC_HF_FIELDS_MEM;
			gpa_hfinfo.hfi = (header_field_info **)g_malloc(sizeof(header_field_info *)*PROTO_PRE_ALLOC_HF_FIELDS_MEM);
		} else {
			gpa_hfinfo.allocated_len += 1000;
			gpa_hfinfo.hfi = (header_field_info **)g_realloc(gpa_hfinfo.hfi,
						   sizeof(header_field_info *)*gpa_hfinfo.allocated_len);
			/*g_warning("gpa_hfinfo.allocated_len %u", gpa_hfinfo.allocated_len);*/
		}
	}
	gpa_hfinfo.hfi[gpa_hfinfo.len] = hfinfo;
	gpa_hfinfo.len++;
	hfinfo->id = gpa_hfinfo.len - 1;

	/* if we have real names, enter this field in the name tree */
	if ((hfinfo->name[0] != 0) && (hfinfo->abbrev[0] != 0 )) {

		header_field_info *same_name_next_hfinfo;
		guchar c;

		/* Check that the filter name (abbreviation) is legal;
		 * it must contain only alphanumerics, '-', "_", and ".". */
		c = proto_check_field_name(hfinfo->abbrev);
		if (c) {
			if (c == '.') {
				fprintf(stderr, "Invalid leading, duplicated or trailing '.' found in filter name '%s'\n", hfinfo->abbrev);
			} else if (g_ascii_isprint(c)) {
				fprintf(stderr, "Invalid character '%c' in filter name '%s'\n", c, hfinfo->abbrev);
			} else {
				fprintf(stderr, "Invalid byte \\%03o in filter name '%s'\n", c, hfinfo->abbrev);
			}
			DISSECTOR_ASSERT_NOT_REACHED();
		}

		/* We allow multiple hfinfo's to be registered under the same
		 * abbreviation. This was done for X.25, as, depending
		 * on whether it's modulo-8 or modulo-128 operation,
		 * some bitfield fields may be in different bits of
		 * a byte, and we want to be able to refer to that field
		 * with one name regardless of whether the packets
		 * are modulo-8 or modulo-128 packets. */

		same_name_hfinfo = NULL;

		g_hash_table_insert(gpa_name_map, (gpointer) (hfinfo->abbrev), hfinfo);
		/* GLIB 2.x - if it is already present
		 * the previous hfinfo with the same name is saved
		 * to same_name_hfinfo by value destroy callback */
		if (same_name_hfinfo) {
			/* There's already a field with this name.
			 * Put the current field *before* that field
			 * in the list of fields with this name, Thus,
			 * we end up with an effectively
			 * doubly-linked-list of same-named hfinfo's,
			 * with the head of the list (stored in the
			 * hash) being the last seen hfinfo.
			 */
			same_name_next_hfinfo =
				same_name_hfinfo->same_name_next;

			hfinfo->same_name_next = same_name_next_hfinfo;
			if (same_name_next_hfinfo)
				same_name_next_hfinfo->same_name_prev_id = hfinfo->id;

			same_name_hfinfo->same_name_next = hfinfo;
			hfinfo->same_name_prev_id = same_name_hfinfo->id;
#ifdef ENABLE_CHECK_FILTER
			while (same_name_hfinfo) {
				if (_ftype_common(hfinfo->type) != _ftype_common(same_name_hfinfo->type))
					g_warning("'%s' exists multiple times with incompatible types: %s and %s", hfinfo->abbrev, ftype_name(hfinfo->type), ftype_name(same_name_hfinfo->type));
				same_name_hfinfo = same_name_hfinfo->same_name_next;
			}
#endif
		}
	}

	return hfinfo->id;
}