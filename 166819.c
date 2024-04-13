proto_tree_new_item(field_info *new_fi, proto_tree *tree,
		    tvbuff_t *tvb, gint start, gint length,
		    guint encoding)
{
	proto_item *pi;
	guint32	    value, n;
	guint64	    value64;
	ws_in4_addr ipv4_value;
	float	    floatval;
	double	    doubleval;
	const char *stringval = NULL;
	nstime_t    time_stamp;
	gboolean    length_error;

	switch (new_fi->hfinfo->type) {
		case FT_NONE:
			/* no value to set for FT_NONE */
			break;

		case FT_PROTOCOL:
			proto_tree_set_protocol_tvb(new_fi, tvb, new_fi->hfinfo->name);
			break;

		case FT_BYTES:
			proto_tree_set_bytes_tvb(new_fi, tvb, start, length);
			break;

		case FT_UINT_BYTES:
			n = get_uint_value(tree, tvb, start, length, encoding);
			proto_tree_set_bytes_tvb(new_fi, tvb, start + length, n);

			/* Instead of calling proto_item_set_len(), since we don't yet
			 * have a proto_item, we set the field_info's length ourselves. */
			new_fi->length = n + length;
			break;

		case FT_BOOLEAN:
			/*
			 * Map all non-zero values to little-endian for
			 * backwards compatibility.
			 */
			if (encoding)
				encoding = ENC_LITTLE_ENDIAN;
			proto_tree_set_boolean(new_fi,
				get_uint64_value(tree, tvb, start, length, encoding));
			break;

		case FT_CHAR:
		/* XXX - make these just FT_UINT? */
		case FT_UINT8:
		case FT_UINT16:
		case FT_UINT24:
		case FT_UINT32:
			if (encoding & ENC_VARINT_PROTOBUF) {
				new_fi->length = tvb_get_varint(tvb, start, (length == -1) ? FT_VARINT_MAX_LEN : length, &value64, encoding);
				new_fi->flags |= FI_VARINT;
				value = (guint32)value64;
			} else if (encoding & ENC_VARINT_QUIC) {
				new_fi->length = tvb_get_varint(tvb, start, (length == -1) ? FT_VARINT_MAX_LEN : length, &value64, encoding);
				value = (guint32)value64;
			} else if (encoding & ENC_VARINT_ZIGZAG) {
				new_fi->length = tvb_get_varint(tvb, start, (length == -1) ? FT_VARINT_MAX_LEN : length, &value64, encoding);
				new_fi->flags |= FI_VARINT;
				value = (guint32)value64;
			}
			else {
				/*
				 * Map all non-zero values to little-endian for
				 * backwards compatibility.
				 */
				if (encoding)
					encoding = ENC_LITTLE_ENDIAN;

				value = get_uint_value(tree, tvb, start, length, encoding);
			}
			proto_tree_set_uint(new_fi, value);
			break;

		case FT_UINT40:
		case FT_UINT48:
		case FT_UINT56:
		case FT_UINT64:

			if (encoding & ENC_VARINT_PROTOBUF) {
				new_fi->length = tvb_get_varint(tvb, start, (length == -1) ? FT_VARINT_MAX_LEN : length, &value64, encoding);
				new_fi->flags |= FI_VARINT;
			} else if (encoding & ENC_VARINT_QUIC) {
				new_fi->length = tvb_get_varint(tvb, start, (length == -1) ? FT_VARINT_MAX_LEN : length, &value64, encoding);
			} else if (encoding & ENC_VARINT_ZIGZAG) {
				new_fi->length = tvb_get_varint(tvb, start, (length == -1) ? FT_VARINT_MAX_LEN : length, &value64, encoding);
				new_fi->flags |= FI_VARINT;
			}
			else {
				/*
				 * Map all other non-zero values to little-endian for
				 * backwards compatibility.
				 */
				if (encoding)
					encoding = ENC_LITTLE_ENDIAN;

				value64 = get_uint64_value(tree, tvb, start, length, encoding);
			}
			proto_tree_set_uint64(new_fi, value64);
			break;

		/* XXX - make these just FT_INT? */
		case FT_INT8:
		case FT_INT16:
		case FT_INT24:
		case FT_INT32:
			/*
			 * Map all non-zero values to little-endian for
			 * backwards compatibility.
			 */
			if (encoding)
				encoding = ENC_LITTLE_ENDIAN;
			proto_tree_set_int(new_fi,
				get_int_value(tree, tvb, start, length, encoding));
			break;

		case FT_INT40:
		case FT_INT48:
		case FT_INT56:
		case FT_INT64:
			/*
			 * Map all non-zero values to little-endian for
			 * backwards compatibility.
			 */
			if (encoding)
				encoding = ENC_LITTLE_ENDIAN;
			proto_tree_set_int64(new_fi,
				get_int64_value(tree, tvb, start, length, encoding));
			break;

		case FT_IPv4:
			/*
			 * Map all non-zero values to little-endian for
			 * backwards compatibility.
			 */
			if (encoding)
				encoding = ENC_LITTLE_ENDIAN;
			if (length != FT_IPv4_LEN) {
				length_error = length < FT_IPv4_LEN ? TRUE : FALSE;
				report_type_length_mismatch(tree, "an IPv4 address", length, length_error);
			}
			ipv4_value = tvb_get_ipv4(tvb, start);
			/*
			 * NOTE: to support code written when
			 * proto_tree_add_item() took a gboolean as its
			 * last argument, with FALSE meaning "big-endian"
			 * and TRUE meaning "little-endian", we treat any
			 * non-zero value of "encoding" as meaning
			 * "little-endian".
			 */
			proto_tree_set_ipv4(new_fi, encoding ? GUINT32_SWAP_LE_BE(ipv4_value) : ipv4_value);
			break;

		case FT_IPXNET:
			if (length != FT_IPXNET_LEN) {
				length_error = length < FT_IPXNET_LEN ? TRUE : FALSE;
				report_type_length_mismatch(tree, "an IPXNET address", length, length_error);
			}
			proto_tree_set_ipxnet(new_fi,
				get_uint_value(tree, tvb, start, FT_IPXNET_LEN, ENC_BIG_ENDIAN));
			break;

		case FT_IPv6:
			if (length != FT_IPv6_LEN) {
				length_error = length < FT_IPv6_LEN ? TRUE : FALSE;
				report_type_length_mismatch(tree, "an IPv6 address", length, length_error);
			}
			proto_tree_set_ipv6_tvb(new_fi, tvb, start, length);
			break;

		case FT_FCWWN:
			if (length != FT_FCWWN_LEN) {
				length_error = length < FT_FCWWN_LEN ? TRUE : FALSE;
				report_type_length_mismatch(tree, "an FCWWN address", length, length_error);
			}
			proto_tree_set_fcwwn_tvb(new_fi, tvb, start, length);
			break;

		case FT_AX25:
			if (length != 7) {
				length_error = length < 7 ? TRUE : FALSE;
				report_type_length_mismatch(tree, "an AX.25 address", length, length_error);
			}
			proto_tree_set_ax25_tvb(new_fi, tvb, start);
			break;

		case FT_VINES:
			if (length != VINES_ADDR_LEN) {
				length_error = length < VINES_ADDR_LEN ? TRUE : FALSE;
				report_type_length_mismatch(tree, "a Vines address", length, length_error);
			}
			proto_tree_set_vines_tvb(new_fi, tvb, start);
			break;

		case FT_ETHER:
			if (length != FT_ETHER_LEN) {
				length_error = length < FT_ETHER_LEN ? TRUE : FALSE;
				report_type_length_mismatch(tree, "a MAC address", length, length_error);
			}
			proto_tree_set_ether_tvb(new_fi, tvb, start);
			break;

		case FT_EUI64:
			/*
			 * Map all non-zero values to little-endian for
			 * backwards compatibility.
			 */
			if (encoding)
				encoding = ENC_LITTLE_ENDIAN;
			if (length != FT_EUI64_LEN) {
				length_error = length < FT_EUI64_LEN ? TRUE : FALSE;
				report_type_length_mismatch(tree, "an EUI-64 address", length, length_error);
			}
			proto_tree_set_eui64_tvb(new_fi, tvb, start, encoding);
			break;
		case FT_GUID:
			/*
			 * Map all non-zero values to little-endian for
			 * backwards compatibility.
			 */
			if (encoding)
				encoding = ENC_LITTLE_ENDIAN;
			if (length != FT_GUID_LEN) {
				length_error = length < FT_GUID_LEN ? TRUE : FALSE;
				report_type_length_mismatch(tree, "a GUID", length, length_error);
			}
			proto_tree_set_guid_tvb(new_fi, tvb, start, encoding);
			break;

		case FT_OID:
		case FT_REL_OID:
			proto_tree_set_oid_tvb(new_fi, tvb, start, length);
			break;

		case FT_SYSTEM_ID:
			proto_tree_set_system_id_tvb(new_fi, tvb, start, length);
			break;

		case FT_FLOAT:
			/*
			 * NOTE: to support code written when
			 * proto_tree_add_item() took a gboolean as its
			 * last argument, with FALSE meaning "big-endian"
			 * and TRUE meaning "little-endian", we treat any
			 * non-zero value of "encoding" as meaning
			 * "little-endian".
			 *
			 * At some point in the future, we might
			 * support non-IEEE-binary floating-point
			 * formats in the encoding as well
			 * (IEEE decimal, System/3x0, VAX).
			 */
			if (encoding)
				encoding = ENC_LITTLE_ENDIAN;
			if (length != 4) {
				length_error = length < 4 ? TRUE : FALSE;
				report_type_length_mismatch(tree, "a single-precision floating point number", length, length_error);
			}
			if (encoding)
				floatval = tvb_get_letohieee_float(tvb, start);
			else
				floatval = tvb_get_ntohieee_float(tvb, start);
			proto_tree_set_float(new_fi, floatval);
			break;

		case FT_DOUBLE:
			/*
			 * NOTE: to support code written when
			 * proto_tree_add_item() took a gboolean as its
			 * last argument, with FALSE meaning "big-endian"
			 * and TRUE meaning "little-endian", we treat any
			 * non-zero value of "encoding" as meaning
			 * "little-endian".
			 *
			 * At some point in the future, we might
			 * support non-IEEE-binary floating-point
			 * formats in the encoding as well
			 * (IEEE decimal, System/3x0, VAX).
			 */
			if (encoding == TRUE)
				encoding = ENC_LITTLE_ENDIAN;
			if (length != 8) {
				length_error = length < 8 ? TRUE : FALSE;
				report_type_length_mismatch(tree, "a double-precision floating point number", length, length_error);
			}
			if (encoding)
				doubleval = tvb_get_letohieee_double(tvb, start);
			else
				doubleval = tvb_get_ntohieee_double(tvb, start);
			proto_tree_set_double(new_fi, doubleval);
			break;

		case FT_STRING:
			stringval = get_string_value(wmem_packet_scope(),
			    tvb, start, length, &length, encoding);
			proto_tree_set_string(new_fi, stringval);

			/* Instead of calling proto_item_set_len(), since we
			 * don't yet have a proto_item, we set the
			 * field_info's length ourselves.
			 *
			 * XXX - our caller can't use that length to
			 * advance an offset unless they arrange that
			 * there always be a protocol tree into which
			 * we're putting this item.
			 */
			new_fi->length = length;
			break;

		case FT_STRINGZ:
			stringval = get_stringz_value(wmem_packet_scope(),
			    tree, tvb, start, length, &length, encoding);
			proto_tree_set_string(new_fi, stringval);

			/* Instead of calling proto_item_set_len(),
			 * since we don't yet have a proto_item, we
			 * set the field_info's length ourselves.
			 *
			 * XXX - our caller can't use that length to
			 * advance an offset unless they arrange that
			 * there always be a protocol tree into which
			 * we're putting this item.
			 */
			new_fi->length = length;
			break;

		case FT_UINT_STRING:
			/*
			 * NOTE: to support code written when
			 * proto_tree_add_item() took a gboolean as its
			 * last argument, with FALSE meaning "big-endian"
			 * and TRUE meaning "little-endian", if the
			 * encoding value is TRUE, treat that as
			 * ASCII with a little-endian length.
			 *
			 * This won't work for code that passes
			 * arbitrary non-zero values; that code
			 * will need to be fixed.
			 */
			if (encoding == TRUE)
				encoding = ENC_ASCII|ENC_LITTLE_ENDIAN;
			stringval = get_uint_string_value(wmem_packet_scope(),
			    tree, tvb, start, length, &length, encoding);
			proto_tree_set_string(new_fi, stringval);

			/* Instead of calling proto_item_set_len(), since we
			 * don't yet have a proto_item, we set the
			 * field_info's length ourselves.
			 *
			 * XXX - our caller can't use that length to
			 * advance an offset unless they arrange that
			 * there always be a protocol tree into which
			 * we're putting this item.
			 */
			new_fi->length = length;
			break;

		case FT_STRINGZPAD:
			stringval = get_stringzpad_value(wmem_packet_scope(),
			    tvb, start, length, &length, encoding);
			proto_tree_set_string(new_fi, stringval);

			/* Instead of calling proto_item_set_len(), since we
			 * don't yet have a proto_item, we set the
			 * field_info's length ourselves.
			 *
			 * XXX - our caller can't use that length to
			 * advance an offset unless they arrange that
			 * there always be a protocol tree into which
			 * we're putting this item.
			 */
			new_fi->length = length;
			break;

		case FT_STRINGZTRUNC:
			stringval = get_stringztrunc_value(wmem_packet_scope(),
			    tvb, start, length, &length, encoding);
			proto_tree_set_string(new_fi, stringval);

			/* Instead of calling proto_item_set_len(), since we
			 * don't yet have a proto_item, we set the
			 * field_info's length ourselves.
			 *
			 * XXX - our caller can't use that length to
			 * advance an offset unless they arrange that
			 * there always be a protocol tree into which
			 * we're putting this item.
			 */
			new_fi->length = length;
			break;

		case FT_ABSOLUTE_TIME:
			/*
			 * Absolute times can be in any of a number of
			 * formats, and they can be big-endian or
			 * little-endian.
			 *
			 * Historically FT_TIMEs were only timespecs;
			 * the only question was whether they were stored
			 * in big- or little-endian format.
			 *
			 * For backwards compatibility, we interpret an
			 * encoding of 1 as meaning "little-endian timespec",
			 * so that passing TRUE is interpreted as that.
			 */
			if (encoding == TRUE)
				encoding = ENC_TIME_SECS_NSECS|ENC_LITTLE_ENDIAN;

			get_time_value(tree, tvb, start, length, encoding, &time_stamp, FALSE);

			proto_tree_set_time(new_fi, &time_stamp);
			break;

		case FT_RELATIVE_TIME:
			/*
			 * Relative times can be in any of a number of
			 * formats, and they can be big-endian or
			 * little-endian.
			 *
			 * Historically FT_TIMEs were only timespecs;
			 * the only question was whether they were stored
			 * in big- or little-endian format.
			 *
			 * For backwards compatibility, we interpret an
			 * encoding of 1 as meaning "little-endian timespec",
			 * so that passing TRUE is interpreted as that.
			 */
			if (encoding == TRUE)
				encoding = ENC_TIME_SECS_NSECS|ENC_LITTLE_ENDIAN;

			get_time_value(tree, tvb, start, length, encoding, &time_stamp, TRUE);

			proto_tree_set_time(new_fi, &time_stamp);
			break;
		case FT_IEEE_11073_SFLOAT:
			if (encoding)
				encoding = ENC_LITTLE_ENDIAN;
			if (length != 2) {
				length_error = length < 2 ? TRUE : FALSE;
				report_type_length_mismatch(tree, "a IEEE 11073 SFLOAT", length, length_error);
			}

			fvalue_set_uinteger(&new_fi->value, tvb_get_guint16(tvb, start, encoding));

			break;
		case FT_IEEE_11073_FLOAT:
			if (encoding)
				encoding = ENC_LITTLE_ENDIAN;
			if (length != 4) {
				length_error = length < 4 ? TRUE : FALSE;
				report_type_length_mismatch(tree, "a IEEE 11073 FLOAT", length, length_error);
			}

			break;
		default:
			REPORT_DISSECTOR_BUG("field %s is of unknown type %d (%s)",
					     new_fi->hfinfo->abbrev,
					     new_fi->hfinfo->type,
					     ftype_name(new_fi->hfinfo->type));
			break;
	}
	FI_SET_FLAG(new_fi, (encoding & ENC_LITTLE_ENDIAN) ? FI_LITTLE_ENDIAN : FI_BIG_ENDIAN);

	/* Don't add new node to proto_tree until now so that any exceptions
	 * raised by a tvbuff access method doesn't leave junk in the proto_tree. */
	/* XXX. wouldn't be better to add this item to tree, with some special flag (FI_EXCEPTION?)
	 *      to know which item caused exception? */
	pi = proto_tree_add_node(tree, new_fi);

	switch (new_fi->hfinfo->type) {

	case FT_STRING:
		detect_trailing_stray_characters(encoding, stringval, length, pi);
		break;

	default:
		break;
	}

	return pi;
}