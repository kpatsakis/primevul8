get_full_length(header_field_info *hfinfo, tvbuff_t *tvb, const gint start,
		gint length, guint item_length, const gint encoding)
{
	guint32 n;

	/*
	 * We need to get the correct item length here.
	 * That's normally done by proto_tree_new_item(),
	 * but we won't be calling it.
	 */
	switch (hfinfo->type) {

	case FT_NONE:
	case FT_PROTOCOL:
	case FT_BYTES:
		/*
		 * The length is the specified length.
		 */
		break;

	case FT_UINT_BYTES:
		n = get_uint_value(NULL, tvb, start, length, encoding);
		item_length += n;
		break;

	/* XXX - make these just FT_UINT? */
	case FT_UINT8:
	case FT_UINT16:
	case FT_UINT24:
	case FT_UINT32:
	case FT_UINT40:
	case FT_UINT48:
	case FT_UINT56:
	case FT_UINT64:
	/* XXX - make these just FT_INT? */
	case FT_INT8:
	case FT_INT16:
	case FT_INT24:
	case FT_INT32:
	case FT_INT40:
	case FT_INT48:
	case FT_INT56:
	case FT_INT64:
		if (encoding & (ENC_VARIANT_MASK)) {
			if (length < -1) {
				report_type_length_mismatch(NULL, "a FT_[U]INT", length, TRUE);
			}
			if (length == -1) {
				guint64 dummy;
				/* This can throw an exception */
				/* XXX - do this without fetching the varint? */
				length = tvb_get_varint(tvb, start, FT_VARINT_MAX_LEN, &dummy, encoding);
				if (length == 0) {
					THROW(ReportedBoundsError);
				}
			}
			item_length = length;
			break;
		}

		/*
		 * The length is the specified length.
		 */
		break;

	case FT_BOOLEAN:
	case FT_CHAR:
	case FT_IPv4:
	case FT_IPXNET:
	case FT_IPv6:
	case FT_FCWWN:
	case FT_AX25:
	case FT_VINES:
	case FT_ETHER:
	case FT_EUI64:
	case FT_GUID:
	case FT_OID:
	case FT_REL_OID:
	case FT_SYSTEM_ID:
	case FT_FLOAT:
	case FT_DOUBLE:
	case FT_STRING:
		/*
		 * The length is the specified length.
		 */
		break;

	case FT_STRINGZ:
		if (length < -1) {
			report_type_length_mismatch(NULL, "a string", length, TRUE);
		}
		if (length == -1) {
			/* This can throw an exception */
			/* XXX - do this without fetching the string? */
			tvb_get_stringz_enc(wmem_packet_scope(), tvb, start, &length, encoding);
		}
		item_length = length;
		break;

	case FT_UINT_STRING:
		n = get_uint_value(NULL, tvb, start, length, encoding & ~ENC_CHARENCODING_MASK);
		item_length += n;
		break;

	case FT_STRINGZPAD:
	case FT_STRINGZTRUNC:
	case FT_ABSOLUTE_TIME:
	case FT_RELATIVE_TIME:
	case FT_IEEE_11073_SFLOAT:
	case FT_IEEE_11073_FLOAT:
		/*
		 * The length is the specified length.
		 */
		break;

	default:
		REPORT_DISSECTOR_BUG("field %s has type %d (%s) not handled in gset_full_length()",
				     hfinfo->abbrev,
				     hfinfo->type,
				     ftype_name(hfinfo->type));
		break;
	}
	return item_length;
}