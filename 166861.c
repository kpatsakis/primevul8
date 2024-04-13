_proto_tree_add_bits_format_value(proto_tree *tree, const int hfindex,
				 tvbuff_t *tvb, const guint bit_offset,
				 const gint no_of_bits, void *value_ptr,
				 gchar *value_str)
{
	gint     offset;
	guint    length;
	guint8   tot_no_bits;
	char    *str;
	guint64  value = 0;
	header_field_info *hf_field;

	/* We do not have to return a value, try to fake it as soon as possible */
	CHECK_FOR_NULL_TREE(tree);
	TRY_TO_FAKE_THIS_ITEM(tree, hfindex, hf_field);

	if (hf_field->bitmask != 0) {
		REPORT_DISSECTOR_BUG("Incompatible use of proto_tree_add_bits_format_value"
				     " with field '%s' (%s) with bitmask != 0",
				     hf_field->abbrev, hf_field->name);
	}

	if (no_of_bits == 0) {
		REPORT_DISSECTOR_BUG("field %s passed to proto_tree_add_bits_format_value() has a bit width of 0",
				     hf_field->abbrev);
	}

	/* Byte align offset */
	offset = bit_offset>>3;

	/*
	 * Calculate the number of octets used to hold the bits
	 */
	tot_no_bits = ((bit_offset&0x7) + no_of_bits);
	length      = tot_no_bits>>3;
	/* If we are using part of the next octet, increase length by 1 */
	if (tot_no_bits & 0x07)
		length++;

	if (no_of_bits < 65) {
		value = tvb_get_bits64(tvb, bit_offset, no_of_bits, ENC_BIG_ENDIAN);
	} else {
		REPORT_DISSECTOR_BUG("field %s passed to proto_tree_add_bits_format_value() has a bit width of %u > 65",
				     hf_field->abbrev, no_of_bits);
		return NULL;
	}

	str = decode_bits_in_field(bit_offset, no_of_bits, value);

	g_strlcat(str, " = ", 256+64);
	g_strlcat(str, hf_field->name, 256+64);

	/*
	 * This function does not receive an actual value but a dimensionless pointer to that value.
	 * For this reason, the type of the header field is examined in order to determine
	 * what kind of value we should read from this address.
	 * The caller of this function must make sure that for the specific header field type the address of
	 * a compatible value is provided.
	 */
	switch (hf_field->type) {
	case FT_BOOLEAN:
		return proto_tree_add_boolean_format(tree, hfindex, tvb, offset, length, *(guint32 *)value_ptr,
						     "%s: %s", str, value_str);
		break;

	case FT_CHAR:
	case FT_UINT8:
	case FT_UINT16:
	case FT_UINT24:
	case FT_UINT32:
		return proto_tree_add_uint_format(tree, hfindex, tvb, offset, length, *(guint32 *)value_ptr,
						  "%s: %s", str, value_str);
		break;

	case FT_UINT40:
	case FT_UINT48:
	case FT_UINT56:
	case FT_UINT64:
		return proto_tree_add_uint64_format(tree, hfindex, tvb, offset, length, *(guint64 *)value_ptr,
						    "%s: %s", str, value_str);
		break;

	case FT_INT8:
	case FT_INT16:
	case FT_INT24:
	case FT_INT32:
		return proto_tree_add_int_format(tree, hfindex, tvb, offset, length, *(gint32 *)value_ptr,
						 "%s: %s", str, value_str);
		break;

	case FT_INT40:
	case FT_INT48:
	case FT_INT56:
	case FT_INT64:
		return proto_tree_add_int64_format(tree, hfindex, tvb, offset, length, *(gint64 *)value_ptr,
						   "%s: %s", str, value_str);
		break;

	case FT_FLOAT:
		return proto_tree_add_float_format(tree, hfindex, tvb, offset, length, *(float *)value_ptr,
						   "%s: %s", str, value_str);
		break;

	default:
		REPORT_DISSECTOR_BUG("field %s has type %d (%s) not handled in proto_tree_add_bits_format_value()",
				     hf_field->abbrev,
				     hf_field->type,
				     ftype_name(hf_field->type));
		return NULL;
		break;
	}
}