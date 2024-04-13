proto_tree_add_checksum(proto_tree *tree, tvbuff_t *tvb, const guint offset,
		const int hf_checksum, const int hf_checksum_status, struct expert_field* bad_checksum_expert,
		packet_info *pinfo, guint32 computed_checksum, const guint encoding, const guint flags)
{
	header_field_info *hfinfo = proto_registrar_get_nth(hf_checksum);
	guint32 checksum;
	guint32 len;
	proto_item* ti = NULL;
	proto_item* ti2;
	gboolean incorrect_checksum = TRUE;

	DISSECTOR_ASSERT_HINT(hfinfo != NULL, "Not passed hfi!");

	switch (hfinfo->type) {
	case FT_UINT8:
		len = 1;
		break;
	case FT_UINT16:
		len = 2;
		break;
	case FT_UINT24:
		len = 3;
		break;
	case FT_UINT32:
		len = 4;
		break;
	default:
		REPORT_DISSECTOR_BUG("field %s is not of type FT_UINT8, FT_UINT16, FT_UINT24, or FT_UINT32",
		    hfinfo->abbrev);
	}

	if (flags & PROTO_CHECKSUM_NOT_PRESENT) {
		ti = proto_tree_add_uint_format_value(tree, hf_checksum, tvb, offset, len, 0, "[missing]");
		proto_item_set_generated(ti);
		if (hf_checksum_status != -1) {
			ti2 = proto_tree_add_uint(tree, hf_checksum_status, tvb, offset, len, PROTO_CHECKSUM_E_NOT_PRESENT);
			proto_item_set_generated(ti2);
		}
		return ti;
	}

	if (flags & PROTO_CHECKSUM_GENERATED) {
		ti = proto_tree_add_uint(tree, hf_checksum, tvb, offset, len, computed_checksum);
		proto_item_set_generated(ti);
	} else {
		ti = proto_tree_add_item_ret_uint(tree, hf_checksum, tvb, offset, len, encoding, &checksum);
		if (flags & PROTO_CHECKSUM_VERIFY) {
			if (flags & (PROTO_CHECKSUM_IN_CKSUM|PROTO_CHECKSUM_ZERO)) {
				if (computed_checksum == 0) {
					proto_item_append_text(ti, " [correct]");
					if (hf_checksum_status != -1) {
						ti2 = proto_tree_add_uint(tree, hf_checksum_status, tvb, offset, 0, PROTO_CHECKSUM_E_GOOD);
						proto_item_set_generated(ti2);
					}
					incorrect_checksum = FALSE;
				} else if (flags & PROTO_CHECKSUM_IN_CKSUM) {
					computed_checksum = in_cksum_shouldbe(checksum, computed_checksum);
				}
			} else {
				if (checksum == computed_checksum) {
					proto_item_append_text(ti, " [correct]");
					if (hf_checksum_status != -1) {
						ti2 = proto_tree_add_uint(tree, hf_checksum_status, tvb, offset, 0, PROTO_CHECKSUM_E_GOOD);
						proto_item_set_generated(ti2);
					}
					incorrect_checksum = FALSE;
				}
			}

			if (incorrect_checksum) {
				if (hf_checksum_status != -1) {
					ti2 = proto_tree_add_uint(tree, hf_checksum_status, tvb, offset, 0, PROTO_CHECKSUM_E_BAD);
					proto_item_set_generated(ti2);
				}
				if (flags & PROTO_CHECKSUM_ZERO) {
					proto_item_append_text(ti, " [incorrect]");
					if (bad_checksum_expert != NULL)
						expert_add_info_format(pinfo, ti, bad_checksum_expert, "%s", expert_get_summary(bad_checksum_expert));
				} else {
					proto_item_append_text(ti, " incorrect, should be 0x%0*x", len*2, computed_checksum);
					if (bad_checksum_expert != NULL)
						expert_add_info_format(pinfo, ti, bad_checksum_expert, "%s [should be 0x%0*x]", expert_get_summary(bad_checksum_expert), len * 2, computed_checksum);
				}
			}
		} else {
			if (hf_checksum_status != -1) {
				proto_item_append_text(ti, " [unverified]");
				ti2 = proto_tree_add_uint(tree, hf_checksum_status, tvb, offset, 0, PROTO_CHECKSUM_E_UNVERIFIED);
				proto_item_set_generated(ti2);
			}
		}
	}

	return ti;
}