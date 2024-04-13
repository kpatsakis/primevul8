get_int64_value(proto_tree *tree, tvbuff_t *tvb, gint start, guint length, const guint encoding)
{
	guint64 value = get_uint64_value(tree, tvb, start, length, encoding);

	switch (length) {
		case 7:
			value = ws_sign_ext64(value, 56);
			break;
		case 6:
			value = ws_sign_ext64(value, 48);
			break;
		case 5:
			value = ws_sign_ext64(value, 40);
			break;
		case 4:
			value = ws_sign_ext64(value, 32);
			break;
		case 3:
			value = ws_sign_ext64(value, 24);
			break;
		case 2:
			value = ws_sign_ext64(value, 16);
			break;
		case 1:
			value = ws_sign_ext64(value, 8);
			break;
	}

	return value;
}