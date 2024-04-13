get_hfi_length(header_field_info *hfinfo, tvbuff_t *tvb, const gint start, gint *length,
		   gint *item_length, const guint encoding)
{
	gint length_remaining;

	/*
	 * We only allow a null tvbuff if the item has a zero length,
	 * i.e. if there's no data backing it.
	 */
	DISSECTOR_ASSERT(tvb != NULL || *length == 0);

	/*
	 * XXX - in some protocols, there are 32-bit unsigned length
	 * fields, so lengths in protocol tree and tvbuff routines
	 * should really be unsigned.  We should have, for those
	 * field types for which "to the end of the tvbuff" makes sense,
	 * additional routines that take no length argument and
	 * add fields that run to the end of the tvbuff.
	 */
	if (*length == -1) {
		/*
		 * For FT_NONE, FT_PROTOCOL, FT_BYTES, FT_STRING,
		 * FT_STRINGZPAD, and FT_STRINGZTRUNC fields, a length
		 * of -1 means "set the length to what remains in the
		 * tvbuff".
		 *
		 * The assumption is either that
		 *
		 *	1) the length of the item can only be determined
		 *	   by dissection (typically true of items with
		 *	   subitems, which are probably FT_NONE or
		 *	   FT_PROTOCOL)
		 *
		 * or
		 *
		 *	2) if the tvbuff is "short" (either due to a short
		 *	   snapshot length or due to lack of reassembly of
		 *	   fragments/segments/whatever), we want to display
		 *	   what's available in the field (probably FT_BYTES
		 *	   or FT_STRING) and then throw an exception later
		 *
		 * or
		 *
		 *	3) the field is defined to be "what's left in the
		 *	   packet"
		 *
		 * so we set the length to what remains in the tvbuff so
		 * that, if we throw an exception while dissecting, it
		 * has what is probably the right value.
		 *
		 * For FT_STRINGZ, it means "the string is null-terminated,
		 * not null-padded; set the length to the actual length
		 * of the string", and if the tvbuff if short, we just
		 * throw an exception.
		 *
		 * For ENC_VARINT_PROTOBUF|ENC_VARINT_QUIC|ENC_VARIANT_ZIGZAG, it means "find the end of the string",
		 * and if the tvbuff if short, we just throw an exception.
		 *
		 * It's not valid for any other type of field.  For those
		 * fields, we treat -1 the same way we treat other
		 * negative values - we assume the length is a Really
		 * Big Positive Number, and throw a ReportedBoundsError
		 * exception, under the assumption that the Really Big
		 * Length would run past the end of the packet.
		 */
		if ((IS_FT_INT(hfinfo->type)) || (IS_FT_UINT(hfinfo->type))) {
			if (encoding & (ENC_VARINT_PROTOBUF|ENC_VARINT_ZIGZAG)) {
				/*
				 * Leave the length as -1, so our caller knows
				 * it was -1.
				 */
				*item_length = *length;
				return;
			} else if (encoding & ENC_VARINT_QUIC) {
				switch (tvb_get_guint8(tvb, start) >> 6)
				{
				case 0: /* 0b00 => 1 byte length (6 bits Usable) */
					*item_length = 1;
					break;
				case 1: /* 0b01 => 2 bytes length (14 bits Usable) */
					*item_length = 2;
					break;
				case 2: /* 0b10 => 4 bytes length (30 bits Usable) */
					*item_length = 4;
					break;
				case 3: /* 0b11 => 8 bytes length (62 bits Usable) */
					*item_length = 8;
					break;
				}
			}
		}

		switch (hfinfo->type) {

		case FT_PROTOCOL:
		case FT_NONE:
		case FT_BYTES:
		case FT_STRING:
		case FT_STRINGZPAD:
		case FT_STRINGZTRUNC:
			/*
			 * We allow FT_PROTOCOLs to be zero-length -
			 * for example, an ONC RPC NULL procedure has
			 * neither arguments nor reply, so the
			 * payload for that protocol is empty.
			 *
			 * We also allow the others to be zero-length -
			 * because that's the way the code has been for a
			 * long, long time.
			 *
			 * However, we want to ensure that the start
			 * offset is not *past* the byte past the end
			 * of the tvbuff: we throw an exception in that
			 * case.
			 */
			*length = tvb_captured_length(tvb) ? tvb_ensure_captured_length_remaining(tvb, start) : 0;
			DISSECTOR_ASSERT(*length >= 0);
			break;

		case FT_STRINGZ:
			/*
			 * Leave the length as -1, so our caller knows
			 * it was -1.
			 */
			break;

		default:
			THROW(ReportedBoundsError);
			DISSECTOR_ASSERT_NOT_REACHED();
		}
		*item_length = *length;
	} else {
		*item_length = *length;
		if (hfinfo->type == FT_PROTOCOL || hfinfo->type == FT_NONE) {
			/*
			 * These types are for interior nodes of the
			 * tree, and don't have data associated with
			 * them; if the length is negative (XXX - see
			 * above) or goes past the end of the tvbuff,
			 * cut it short at the end of the tvbuff.
			 * That way, if this field is selected in
			 * Wireshark, we don't highlight stuff past
			 * the end of the data.
			 */
			/* XXX - what to do, if we don't have a tvb? */
			if (tvb) {
				length_remaining = tvb_captured_length_remaining(tvb, start);
				if (*item_length < 0 ||
					(*item_length > 0 &&
					  (length_remaining < *item_length)))
					*item_length = length_remaining;
			}
		}
		if (*item_length < 0) {
			THROW(ReportedBoundsError);
		}
	}
}