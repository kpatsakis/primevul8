get_int_value(proto_tree *tree, tvbuff_t *tvb, gint offset, gint length, const guint encoding)
{
	gint32 value;
	gboolean length_error;

	switch (length) {

	case 1:
		value = tvb_get_gint8(tvb, offset);
		break;

	case 2:
		value = encoding ? tvb_get_letohis(tvb, offset)
				 : tvb_get_ntohis(tvb, offset);
		break;

	case 3:
		value = encoding ? tvb_get_letohi24(tvb, offset)
				 : tvb_get_ntohi24(tvb, offset);
		break;

	case 4:
		value = encoding ? tvb_get_letohil(tvb, offset)
				 : tvb_get_ntohil(tvb, offset);
		break;

	default:
		if (length < 1) {
			length_error = TRUE;
			value = 0;
		} else {
			length_error = FALSE;
			value = encoding ? tvb_get_letohil(tvb, offset)
					 : tvb_get_ntohil(tvb, offset);
		}
		report_type_length_mismatch(tree, "a signed integer", length, length_error);
		break;
	}
	return value;
}