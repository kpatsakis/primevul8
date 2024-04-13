ws_type_to_elastic(guint type _U_)
{
	switch(type) {
		case FT_UINT16:
		case FT_INT16:
		case FT_INT32:
		case FT_UINT24:
		case FT_INT24:
			return "integer";
		case FT_INT8:
		case FT_UINT8:
			return "short";
		case FT_FRAMENUM:
		case FT_UINT32:
		case FT_UINT40:
		case FT_UINT48:
		case FT_UINT56:
		case FT_UINT64: // Actually it's not handled by 'long' elastic type.
		case FT_INT48:
		case FT_INT64:
			return "long";
		case FT_FLOAT:
		case FT_DOUBLE:
			return "float";
		case FT_IPv6:
		case FT_IPv4:
			return "ip";
		case FT_ABSOLUTE_TIME:
		case FT_RELATIVE_TIME:
			return "date";
		case FT_BYTES:
		case FT_UINT_BYTES:
			return "byte";
		case FT_BOOLEAN:
			return "boolean";
		default:
			return NULL;
	}
}