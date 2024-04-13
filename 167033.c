proto_tree_add_split_bits_item_ret_val(proto_tree *tree, const int hfindex, tvbuff_t *tvb,
				       const guint bit_offset, const crumb_spec_t *crumb_spec,
				       guint64 *return_value)
{
	proto_item *pi;
	gint        no_of_bits;
	gint        octet_offset;
	guint       mask_initial_bit_offset;
	guint       mask_greatest_bit_offset;
	guint       octet_length;
	guint8      i;
	char        bf_str[256];
	char        lbl_str[ITEM_LABEL_LENGTH];
	guint64     value;
	guint64     composite_bitmask;
	guint64     composite_bitmap;

	header_field_info       *hf_field;
	const true_false_string *tfstring;

	/* We can't fake it just yet. We have to fill in the 'return_value' parameter */
	PROTO_REGISTRAR_GET_NTH(hfindex, hf_field);

	if (hf_field->bitmask != 0) {
		REPORT_DISSECTOR_BUG("Incompatible use of proto_tree_add_split_bits_item_ret_val"
				     " with field '%s' (%s) with bitmask != 0",
				     hf_field->abbrev, hf_field->name);
	}

	mask_initial_bit_offset = bit_offset % 8;

	no_of_bits = 0;
	value      = 0;
	i          = 0;
	mask_greatest_bit_offset = 0;
	composite_bitmask        = 0;
	composite_bitmap         = 0;

	while (crumb_spec[i].crumb_bit_length != 0) {
		guint64 crumb_mask, crumb_value;
		guint8	crumb_end_bit_offset;

		crumb_value = tvb_get_bits64(tvb,
					     bit_offset + crumb_spec[i].crumb_bit_offset,
					     crumb_spec[i].crumb_bit_length,
					     ENC_BIG_ENDIAN);
		value      += crumb_value;
		no_of_bits += crumb_spec[i].crumb_bit_length;
		DISSECTOR_ASSERT_HINT(no_of_bits <= 64, "a value larger than 64 bits cannot be represented");

		/* The bitmask is 64 bit, left-aligned, starting at the first bit of the
		   octet containing the initial offset.
		   If the mask is beyond 32 bits, then give up on bit map display.
		   This could be improved in future, probably showing a table
		   of 32 or 64 bits per row */
		if (mask_greatest_bit_offset < 32) {
			crumb_end_bit_offset = mask_initial_bit_offset
				+ crumb_spec[i].crumb_bit_offset
				+ crumb_spec[i].crumb_bit_length;
			crumb_mask = (G_GUINT64_CONSTANT(1) << crumb_spec[i].crumb_bit_length) - 1;

			if (crumb_end_bit_offset > mask_greatest_bit_offset) {
				mask_greatest_bit_offset = crumb_end_bit_offset;
			}
			/* Currently the bitmap of the crumbs are only shown if
			 * smaller than 32 bits. Do not bother calculating the
			 * mask if it is larger than that. */
			if (crumb_end_bit_offset <= 32) {
				composite_bitmask |= (crumb_mask  << (64 - crumb_end_bit_offset));
				composite_bitmap  |= (crumb_value << (64 - crumb_end_bit_offset));
			}
		}
		/* Shift left for the next segment */
		value <<= crumb_spec[++i].crumb_bit_length;
	}

	/* Sign extend for signed types */
	switch (hf_field->type) {
		case FT_INT8:
		case FT_INT16:
		case FT_INT24:
		case FT_INT32:
		case FT_INT40:
		case FT_INT48:
		case FT_INT56:
		case FT_INT64:
			value = ws_sign_ext64(value, no_of_bits);
			break;
		default:
			break;
	}

	if (return_value) {
		*return_value = value;
	}

	/* Coast clear. Try and fake it */
	CHECK_FOR_NULL_TREE(tree);
	TRY_TO_FAKE_THIS_ITEM(tree, hfindex, hf_field);

	/* initialise the format string */
	bf_str[0] = '\0';

	octet_offset = bit_offset >> 3;

	/* Round up mask length to nearest octet */
	octet_length = ((mask_greatest_bit_offset + 7) >> 3);
	mask_greatest_bit_offset = octet_length << 3;

	/* As noted above, we currently only produce a bitmap if the crumbs span less than 4 octets of the tvb.
	   It would be a useful enhancement to eliminate this restriction. */
	if (mask_greatest_bit_offset > 0 && mask_greatest_bit_offset <= 32) {
		other_decode_bitfield_value(bf_str,
					    (guint32)(composite_bitmap  >> (64 - mask_greatest_bit_offset)),
					    (guint32)(composite_bitmask >> (64 - mask_greatest_bit_offset)),
					    mask_greatest_bit_offset);
	} else {
		/* If the bitmask is too large, try to describe its contents. */
		g_snprintf(bf_str, sizeof(bf_str), "%d bits", no_of_bits);
	}

	switch (hf_field->type) {
	case FT_BOOLEAN: /* it is a bit odd to have a boolean encoded as split-bits, but possible, I suppose? */
		/* Boolean field */
		tfstring = &tfs_true_false;
		if (hf_field->strings)
			tfstring = (const true_false_string *) hf_field->strings;
		return proto_tree_add_boolean_format(tree, hfindex,
						     tvb, octet_offset, octet_length, (guint32)value,
						     "%s = %s: %s",
						     bf_str, hf_field->name, tfs_get_string(!!value, tfstring));
		break;

	case FT_CHAR:
		pi = proto_tree_add_uint(tree, hfindex, tvb, octet_offset, octet_length, (guint32)value);
		fill_label_char(PITEM_FINFO(pi), lbl_str);
		break;

	case FT_UINT8:
	case FT_UINT16:
	case FT_UINT24:
	case FT_UINT32:
		pi = proto_tree_add_uint(tree, hfindex, tvb, octet_offset, octet_length, (guint32)value);
		fill_label_number(PITEM_FINFO(pi), lbl_str, FALSE);
		break;

	case FT_INT8:
	case FT_INT16:
	case FT_INT24:
	case FT_INT32:
		pi = proto_tree_add_int(tree, hfindex, tvb, octet_offset, octet_length, (gint32)value);
		fill_label_number(PITEM_FINFO(pi), lbl_str, TRUE);
		break;

	case FT_UINT40:
	case FT_UINT48:
	case FT_UINT56:
	case FT_UINT64:
		pi = proto_tree_add_uint64(tree, hfindex, tvb, octet_offset, octet_length, value);
		fill_label_number64(PITEM_FINFO(pi), lbl_str, FALSE);
		break;

	case FT_INT40:
	case FT_INT48:
	case FT_INT56:
	case FT_INT64:
		pi = proto_tree_add_int64(tree, hfindex, tvb, octet_offset, octet_length, (gint64)value);
		fill_label_number64(PITEM_FINFO(pi), lbl_str, TRUE);
		break;

	default:
		REPORT_DISSECTOR_BUG("field %s has type %d (%s) not handled in proto_tree_add_split_bits_item_ret_val()",
				     hf_field->abbrev,
				     hf_field->type,
				     ftype_name(hf_field->type));
		return NULL;
		break;
	}
	proto_item_set_text(pi, "%s = %s", bf_str, lbl_str);
	return pi;
}