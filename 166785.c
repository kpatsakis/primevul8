proto_item_add_bitmask_tree(proto_item *item, tvbuff_t *tvb, const int offset,
			    const int len, const gint ett, int * const *fields,
			    const int flags, gboolean first,
			    gboolean use_parent_tree,
			    proto_tree* tree, guint64 value)
{
	guint64            available_bits = G_MAXUINT64;
	guint64            bitmask = 0;
	guint64            tmpval;
	header_field_info *hf;
	guint32            integer32;
	gint               bit_offset;
	gint               no_of_bits;

	if (len < 0 || len > 8)
		g_assert_not_reached();
	/**
	 * packet-frame.c uses len=0 since the value is taken from the packet
	 * metadata, not the packet bytes. In that case, assume that all bits
	 * in the provided value are valid.
	 */
	if (len > 0) {
		available_bits >>= (8 - (guint)len)*8;
	}

	if (use_parent_tree == FALSE)
		tree = proto_item_add_subtree(item, ett);

	while (*fields) {
		guint64 present_bits;
		PROTO_REGISTRAR_GET_NTH(**fields,hf);
		DISSECTOR_ASSERT_HINT(hf->bitmask != 0, hf->abbrev);

		bitmask |= hf->bitmask;

		/* Skip fields that aren't fully present */
		present_bits = available_bits & hf->bitmask;
		if (present_bits != hf->bitmask) {
			fields++;
			continue;
		}

		switch (hf->type) {
		case FT_CHAR:
		case FT_UINT8:
		case FT_UINT16:
		case FT_UINT24:
		case FT_UINT32:
			proto_tree_add_uint(tree, **fields, tvb, offset, len, (guint32)value);
			break;

		case FT_INT8:
		case FT_INT16:
		case FT_INT24:
		case FT_INT32:
			proto_tree_add_int(tree, **fields, tvb, offset, len, (gint32)value);
			break;

		case FT_UINT40:
		case FT_UINT48:
		case FT_UINT56:
		case FT_UINT64:
			proto_tree_add_uint64(tree, **fields, tvb, offset, len, value);
			break;

		case FT_INT40:
		case FT_INT48:
		case FT_INT56:
		case FT_INT64:
			proto_tree_add_int64(tree, **fields, tvb, offset, len, (gint64)value);
			break;

		case FT_BOOLEAN:
			proto_tree_add_boolean64(tree, **fields, tvb, offset, len, value);
			break;

		default:
			REPORT_DISSECTOR_BUG("field %s has type %d (%s) not handled in proto_item_add_bitmask_tree()",
					     hf->abbrev,
					     hf->type,
					     ftype_name(hf->type));
			break;
		}
		if (flags & BMT_NO_APPEND) {
			fields++;
			continue;
		}
		tmpval = (value & hf->bitmask) >> hfinfo_bitshift(hf);

		switch (hf->type) {
		case FT_CHAR:
			if (hf->display == BASE_CUSTOM) {
				gchar lbl[ITEM_LABEL_LENGTH];
				const custom_fmt_func_t fmtfunc = (const custom_fmt_func_t)hf->strings;

				DISSECTOR_ASSERT(fmtfunc);
				fmtfunc(lbl, (guint32) tmpval);
				proto_item_append_text(item, "%s%s: %s", first ? "" : ", ",
						hf->name, lbl);
				first = FALSE;
			}
			else if (hf->strings) {
				proto_item_append_text(item, "%s%s: %s", first ? "" : ", ",
						       hf->name, hf_try_val_to_str_const((guint32) tmpval, hf, "Unknown"));
				first = FALSE;
			}
			else if (!(flags & BMT_NO_INT)) {
				char buf[32];
				const char *out;

				if (!first) {
					proto_item_append_text(item, ", ");
				}

				out = hfinfo_char_value_format(hf, buf, (guint32) tmpval);
				proto_item_append_text(item, "%s: %s", hf->name, out);
				first = FALSE;
			}

			break;

		case FT_UINT8:
		case FT_UINT16:
		case FT_UINT24:
		case FT_UINT32:
			if (hf->display == BASE_CUSTOM) {
				gchar lbl[ITEM_LABEL_LENGTH];
				const custom_fmt_func_t fmtfunc = (const custom_fmt_func_t)hf->strings;

				DISSECTOR_ASSERT(fmtfunc);
				fmtfunc(lbl, (guint32) tmpval);
				proto_item_append_text(item, "%s%s: %s", first ? "" : ", ",
						hf->name, lbl);
				first = FALSE;
			}
			else if ((hf->strings) &&(!(hf->display & BASE_UNIT_STRING))) {
				proto_item_append_text(item, "%s%s: %s", first ? "" : ", ",
										hf->name, hf_try_val_to_str_const((guint32) tmpval, hf, "Unknown"));
				first = FALSE;
			}
			else if (!(flags & BMT_NO_INT)) {
				char buf[32];
				const char *out;

				if (!first) {
					proto_item_append_text(item, ", ");
				}

				out = hfinfo_number_value_format(hf, buf, (guint32) tmpval);
				if (hf->display & BASE_UNIT_STRING) {
					proto_item_append_text(item, "%s: %s%s", hf->name, out, unit_name_string_get_value((guint32) tmpval, (const unit_name_string*)hf->strings));
				} else {
					proto_item_append_text(item, "%s: %s", hf->name, out);
				}
				first = FALSE;
			}

			break;

		case FT_INT8:
		case FT_INT16:
		case FT_INT24:
		case FT_INT32:
			integer32 = (guint32) tmpval;
			if (hf->bitmask) {
				no_of_bits = ws_count_ones(hf->bitmask);
				integer32 = ws_sign_ext32(integer32, no_of_bits);
			}
			if (hf->display == BASE_CUSTOM) {
				gchar lbl[ITEM_LABEL_LENGTH];
				const custom_fmt_func_t fmtfunc = (const custom_fmt_func_t)hf->strings;

				DISSECTOR_ASSERT(fmtfunc);
				fmtfunc(lbl, (gint32) integer32);
				proto_item_append_text(item, "%s%s: %s", first ? "" : ", ",
						hf->name, lbl);
				first = FALSE;
			}
			else if ((hf->strings) &&(!(hf->display & BASE_UNIT_STRING))) {
				proto_item_append_text(item, "%s%s: %s", first ? "" : ", ",
						hf->name, hf_try_val_to_str_const((gint32) integer32, hf, "Unknown"));
				first = FALSE;
			}
			else if (!(flags & BMT_NO_INT)) {
				char buf[32];
				const char *out;

				if (!first) {
					proto_item_append_text(item, ", ");
				}

				out = hfinfo_number_value_format(hf, buf, (gint32) integer32);
				if ((hf->strings) &&(!(hf->display & BASE_UNIT_STRING))) {
					proto_item_append_text(item, "%s: %s%s", hf->name, out, unit_name_string_get_value((guint32) tmpval, (const unit_name_string*)hf->strings));
				} else {
					proto_item_append_text(item, "%s: %s", hf->name, out);
				}
				first = FALSE;
			}

			break;

		case FT_UINT40:
		case FT_UINT48:
		case FT_UINT56:
		case FT_UINT64:
			if (hf->display == BASE_CUSTOM) {
				gchar lbl[ITEM_LABEL_LENGTH];
				const custom_fmt_func_64_t fmtfunc = (const custom_fmt_func_64_t)hf->strings;

				DISSECTOR_ASSERT(fmtfunc);
				fmtfunc(lbl, tmpval);
				proto_item_append_text(item, "%s%s: %s", first ? "" : ", ",
						hf->name, lbl);
				first = FALSE;
			}
			else if (hf->strings) {
				proto_item_append_text(item, "%s%s: %s", first ? "" : ", ",
						hf->name, hf_try_val64_to_str_const(tmpval, hf, "Unknown"));
				first = FALSE;
			}
			else if (!(flags & BMT_NO_INT)) {
				char buf[48];
				const char *out;

				if (!first) {
					proto_item_append_text(item, ", ");
				}

				out = hfinfo_number_value_format64(hf, buf, tmpval);
				proto_item_append_text(item, "%s: %s", hf->name, out);
				first = FALSE;
			}

			break;

		case FT_INT40:
		case FT_INT48:
		case FT_INT56:
		case FT_INT64:
			if (hf->bitmask) {
				no_of_bits = ws_count_ones(hf->bitmask);
				tmpval = ws_sign_ext64(tmpval, no_of_bits);
			}
			if (hf->display == BASE_CUSTOM) {
				gchar lbl[ITEM_LABEL_LENGTH];
				const custom_fmt_func_64_t fmtfunc = (const custom_fmt_func_64_t)hf->strings;

				DISSECTOR_ASSERT(fmtfunc);
				fmtfunc(lbl, (gint64) tmpval);
				proto_item_append_text(item, "%s%s: %s", first ? "" : ", ",
						hf->name, lbl);
				first = FALSE;
			}
			else if (hf->strings) {
				proto_item_append_text(item, "%s%s: %s", first ? "" : ", ",
						hf->name, hf_try_val64_to_str_const((gint64) tmpval, hf, "Unknown"));
				first = FALSE;
			}
			else if (!(flags & BMT_NO_INT)) {
				char buf[48];
				const char *out;

				if (!first) {
					proto_item_append_text(item, ", ");
				}

				out = hfinfo_number_value_format64(hf, buf, (gint64) tmpval);
				proto_item_append_text(item, "%s: %s", hf->name, out);
				first = FALSE;
			}

			break;

		case FT_BOOLEAN:
			if (hf->strings && !(flags & BMT_NO_TFS)) {
				/* If we have true/false strings, emit full - otherwise messages
				   might look weird */
				const struct true_false_string *tfs =
					(const struct true_false_string *)hf->strings;

				if (tmpval) {
					proto_item_append_text(item, "%s%s: %s", first ? "" : ", ",
							hf->name, tfs->true_string);
					first = FALSE;
				} else if (!(flags & BMT_NO_FALSE)) {
					proto_item_append_text(item, "%s%s: %s", first ? "" : ", ",
							hf->name, tfs->false_string);
					first = FALSE;
				}
			} else if (hf->bitmask & value) {
				/* If the flag is set, show the name */
				proto_item_append_text(item, "%s%s", first ? "" : ", ", hf->name);
				first = FALSE;
			}
			break;
		default:
			REPORT_DISSECTOR_BUG("field %s has type %d (%s) not handled in proto_item_add_bitmask_tree()",
					     hf->abbrev,
					     hf->type,
					     ftype_name(hf->type));
			break;
		}

		fields++;
	}

	/* XXX: We don't pass the hfi into this function. Perhaps we should,
	 * but then again most dissectors don't set the bitmask field for
	 * the higher level bitmask hfi, so calculate the bitmask from the
	 * fields present. */
	if (item) {
		bit_offset = len*8 - 1 - ws_ilog2(bitmask);
		no_of_bits = ws_ilog2(bitmask) - ws_ctz(bitmask) + 1;
		FI_SET_FLAG(PNODE_FINFO(item), FI_BITS_OFFSET(bit_offset));
		FI_SET_FLAG(PNODE_FINFO(item), FI_BITS_SIZE(no_of_bits));
	}
	return first;
}