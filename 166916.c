get_uint64_value(proto_tree *tree, tvbuff_t *tvb, gint offset, guint length, const guint encoding)
{
	guint64 value;
	gboolean length_error;

	switch (length) {

	case 1:
		value = tvb_get_guint8(tvb, offset);
		break;

	case 2:
		value = (encoding & ENC_LITTLE_ENDIAN) ? tvb_get_letohs(tvb, offset)
						       : tvb_get_ntohs(tvb, offset);
		break;

	case 3:
		value = (encoding & ENC_LITTLE_ENDIAN) ? tvb_get_letoh24(tvb, offset)
						       : tvb_get_ntoh24(tvb, offset);
		break;

	case 4:
		value = (encoding & ENC_LITTLE_ENDIAN) ? tvb_get_letohl(tvb, offset)
						       : tvb_get_ntohl(tvb, offset);
		break;

	case 5:
		value = (encoding & ENC_LITTLE_ENDIAN) ? tvb_get_letoh40(tvb, offset)
						       : tvb_get_ntoh40(tvb, offset);
		break;

	case 6:
		value = (encoding & ENC_LITTLE_ENDIAN) ? tvb_get_letoh48(tvb, offset)
						       : tvb_get_ntoh48(tvb, offset);
		break;

	case 7:
		value = (encoding & ENC_LITTLE_ENDIAN) ? tvb_get_letoh56(tvb, offset)
						       : tvb_get_ntoh56(tvb, offset);
		break;

	case 8:
		value = (encoding & ENC_LITTLE_ENDIAN) ? tvb_get_letoh64(tvb, offset)
						       : tvb_get_ntoh64(tvb, offset);
		break;

	default:
		if (length < 1) {
			length_error = TRUE;
			value = 0;
		} else {
			length_error = FALSE;
			value = (encoding & ENC_LITTLE_ENDIAN) ? tvb_get_letoh64(tvb, offset)
							       : tvb_get_ntoh64(tvb, offset);
		}
		report_type_length_mismatch(tree, "an unsigned integer", length, length_error);
		break;
	}
	return value;
}