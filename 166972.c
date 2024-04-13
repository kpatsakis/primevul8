get_uint_value(proto_tree *tree, tvbuff_t *tvb, gint offset, gint length, const guint encoding)
{
	guint32 value;
	gboolean length_error;

	switch (length) {

	case 1:
		value = tvb_get_guint8(tvb, offset);
		if (encoding & ENC_ZIGBEE) {
			if (value == 0xFF) { /* Invalid Zigbee length, set to 0 */
				value = 0;
			}
		}
		break;

	case 2:
		value = (encoding & ENC_LITTLE_ENDIAN) ? tvb_get_letohs(tvb, offset)
						       : tvb_get_ntohs(tvb, offset);
		if (encoding & ENC_ZIGBEE) {
			if (value == 0xFFFF) { /* Invalid Zigbee length, set to 0 */
				value = 0;
			}
		}
		break;

	case 3:
		value = (encoding & ENC_LITTLE_ENDIAN) ? tvb_get_letoh24(tvb, offset)
						       : tvb_get_ntoh24(tvb, offset);
		break;

	case 4:
		value = (encoding & ENC_LITTLE_ENDIAN) ? tvb_get_letohl(tvb, offset)
						       : tvb_get_ntohl(tvb, offset);
		break;

	default:
		if (length < 1) {
			length_error = TRUE;
			value = 0;
		} else {
			length_error = FALSE;
			value = (encoding & ENC_LITTLE_ENDIAN) ? tvb_get_letohl(tvb, offset)
							       : tvb_get_ntohl(tvb, offset);
		}
		report_type_length_mismatch(tree, "an unsigned integer", length, length_error);
		break;
	}
	return value;
}