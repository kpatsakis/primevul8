proto_custom_set(proto_tree* tree, GSList *field_ids, gint occurrence,
		 gchar *result, gchar *expr, const int size)
{
	guint32             number;
	guint64             number64;
	guint8             *bytes;

	int                 len, prev_len, last, i, offset_r = 0, offset_e = 0, label_len;
	GPtrArray          *finfos;
	field_info         *finfo         = NULL;
	header_field_info*  hfinfo;
	const gchar        *abbrev        = NULL;

	const char *hf_str_val;
	char number_buf[48];
	const char *number_out;
	char *str;
	int *field_idx;
	int field_id;
	int ii = 0;

	g_assert(field_ids != NULL);
	while ((field_idx = (int *) g_slist_nth_data(field_ids, ii++))) {
		field_id = *field_idx;
		PROTO_REGISTRAR_GET_NTH((guint)field_id, hfinfo);

		/* do we need to rewind ? */
		if (!hfinfo)
			return "";

		if (occurrence < 0) {
			/* Search other direction */
			while (hfinfo->same_name_prev_id != -1) {
				PROTO_REGISTRAR_GET_NTH(hfinfo->same_name_prev_id, hfinfo);
			}
		}

		prev_len = 0; /* Reset handled occurrences */

		while (hfinfo) {
			finfos = proto_get_finfo_ptr_array(tree, hfinfo->id);

			if (!finfos || !(len = g_ptr_array_len(finfos))) {
				if (occurrence < 0) {
					hfinfo = hfinfo->same_name_next;
				} else {
					hfinfo = hfinfo_same_name_get_prev(hfinfo);
				}
				continue;
			}

			/* Are there enough occurrences of the field? */
			if (((occurrence - prev_len) > len) || ((occurrence + prev_len) < -len)) {
				if (occurrence < 0) {
					hfinfo = hfinfo->same_name_next;
				} else {
					hfinfo = hfinfo_same_name_get_prev(hfinfo);
				}
				prev_len += len;
				continue;
			}

			/* Calculate single index or set outer bounderies */
			if (occurrence < 0) {
				i = occurrence + len + prev_len;
				last = i;
			} else if (occurrence > 0) {
				i = occurrence - 1 - prev_len;
				last = i;
			} else {
				i = 0;
				last = len - 1;
			}

			prev_len += len; /* Count handled occurrences */

			while (i <= last) {
				finfo = (field_info *)g_ptr_array_index(finfos, i);

				if (offset_r && (offset_r < (size - 2)))
					result[offset_r++] = ',';

				if (offset_e && (offset_e < (size - 2)))
					expr[offset_e++] = ',';

				switch (hfinfo->type) {

					case FT_BOOLEAN:
						offset_r += proto_item_fill_display_label(finfo, result+offset_r, size-offset_r);

						number64 = fvalue_get_uinteger64(&finfo->value);
						offset_e += protoo_strlcpy(expr+offset_e,
								number64 ? "1" : "0", size-offset_e);
						break;

					case FT_CHAR:
						offset_r += proto_item_fill_display_label(finfo, result+offset_r, size-offset_r);

						number = fvalue_get_uinteger(&finfo->value);

						if (hfinfo->strings && FIELD_DISPLAY(hfinfo->display) == BASE_NONE) {
							hf_str_val = hf_try_val_to_str(number, hfinfo);
							g_snprintf(expr+offset_e, size-offset_e, "\"%s\"", hf_str_val);
						} else {
							number_out = hfinfo_char_value_format(hfinfo, number_buf, number);

							g_strlcpy(expr+offset_e, number_out, size-offset_e);
						}

						offset_e = (int)strlen(expr);
						break;

						/* XXX - make these just FT_NUMBER? */
					case FT_INT8:
					case FT_INT16:
					case FT_INT24:
					case FT_INT32:
					case FT_UINT8:
					case FT_UINT16:
					case FT_UINT24:
					case FT_UINT32:
					case FT_FRAMENUM:
						offset_r += proto_item_fill_display_label(finfo, result+offset_r, size-offset_r);

						hf_str_val = NULL;
						number = IS_FT_INT(hfinfo->type) ?
							(guint32) fvalue_get_sinteger(&finfo->value) :
							fvalue_get_uinteger(&finfo->value);

						if (hfinfo->strings && hfinfo->type != FT_FRAMENUM) {
							hf_str_val = hf_try_val_to_str(number, hfinfo);
						}

						if (hf_str_val && FIELD_DISPLAY(hfinfo->display) == BASE_NONE) {

							hf_str_val = hf_try_val_to_str(number, hfinfo);
							g_snprintf(expr+offset_e, size-offset_e, "\"%s\"", hf_str_val);
						} else {
							number_out = hfinfo_numeric_value_format(hfinfo, number_buf, number);

							g_strlcpy(expr+offset_e, number_out, size-offset_e);
						}

						offset_e = (int)strlen(expr);
						break;

					case FT_INT40:
					case FT_INT48:
					case FT_INT56:
					case FT_INT64:
					case FT_UINT40:
					case FT_UINT48:
					case FT_UINT56:
					case FT_UINT64:
						offset_r += proto_item_fill_display_label(finfo, result+offset_r, size-offset_r);

						hf_str_val = NULL;
						number64 = IS_FT_INT(hfinfo->type) ?
							(guint64) fvalue_get_sinteger64(&finfo->value) :
							fvalue_get_uinteger64(&finfo->value);

						if (hfinfo->strings && hfinfo->type != FT_FRAMENUM) {
							hf_str_val = hf_try_val64_to_str(number64, hfinfo);
						}

						if (hf_str_val && FIELD_DISPLAY(hfinfo->display) == BASE_NONE) {
							g_snprintf(expr+offset_e, size-offset_e, "\"%s\"", hf_str_val);
						} else {
							number_out = hfinfo_numeric_value_format64(hfinfo, number_buf, number64);

							g_strlcpy(expr+offset_e, number_out, size-offset_e);
						}

						offset_e = (int)strlen(expr);
						break;

					case FT_REL_OID:
						offset_r += proto_item_fill_display_label(finfo, result+offset_r, size-offset_r);

						bytes = (guint8 *)fvalue_get(&finfo->value);
						str = rel_oid_encoded2string(NULL, bytes, fvalue_length(&finfo->value));
						offset_e += protoo_strlcpy(expr+offset_e, str, size-offset_e);
						wmem_free(NULL, str);
						break;

					case FT_OID:
						offset_r += proto_item_fill_display_label(finfo, result+offset_r, size-offset_r);

						bytes = (guint8 *)fvalue_get(&finfo->value);
						str = oid_encoded2string(NULL, bytes, fvalue_length(&finfo->value));
						offset_e += protoo_strlcpy(expr+offset_e, str, size-offset_e);
						wmem_free(NULL, str);
						break;

					case FT_SYSTEM_ID:
						label_len = proto_item_fill_display_label(finfo, result+offset_r, size-offset_r);

						offset_e += protoo_strlcpy(expr+offset_e, result+offset_r, size-offset_e);

						offset_r += label_len;
						break;

					default:
						offset_r += proto_item_fill_display_label(finfo, result+offset_r, size-offset_r);
						break;
				}
				i++;
			}

			switch (hfinfo->type) {

				case FT_BOOLEAN:
				case FT_CHAR:
				case FT_UINT8:
				case FT_UINT16:
				case FT_UINT24:
				case FT_UINT32:
				case FT_UINT40:
				case FT_UINT48:
				case FT_UINT56:
				case FT_UINT64:
				case FT_FRAMENUM:
				case FT_INT8:
				case FT_INT16:
				case FT_INT24:
				case FT_INT32:
				case FT_INT40:
				case FT_INT48:
				case FT_INT56:
				case FT_INT64:
				case FT_OID:
				case FT_REL_OID:
				case FT_SYSTEM_ID:
					/* for these types, "expr" is filled in the loop above */
					break;

				default:
					/* for all others, just copy "result" to "expr" */
					g_strlcpy(expr, result, size);
					break;
			}

			if (!abbrev) {
				/* Store abbrev for return value */
				abbrev = hfinfo->abbrev;
			}

			if (occurrence == 0) {
				/* Fetch next hfinfo with same name (abbrev) */
				hfinfo = hfinfo_same_name_get_prev(hfinfo);
			} else {
				hfinfo = NULL;
			}
		}
	}

	return abbrev ? abbrev : "";
}