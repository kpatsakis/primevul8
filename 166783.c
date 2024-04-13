_ftype_common(enum ftenum type)
{
	switch (type) {
		case FT_INT8:
		case FT_INT16:
		case FT_INT24:
		case FT_INT32:
			return FT_INT32;

		case FT_CHAR:
		case FT_UINT8:
		case FT_UINT16:
		case FT_UINT24:
		case FT_UINT32:
		case FT_IPXNET:
		case FT_FRAMENUM:
			return FT_UINT32;

		case FT_UINT64:
		case FT_EUI64:
			return FT_UINT64;

		case FT_STRING:
		case FT_STRINGZ:
		case FT_UINT_STRING:
			return FT_STRING;

		case FT_FLOAT:
		case FT_DOUBLE:
			return FT_DOUBLE;

		case FT_BYTES:
		case FT_UINT_BYTES:
		case FT_ETHER:
		case FT_OID:
			return FT_BYTES;

		case FT_ABSOLUTE_TIME:
		case FT_RELATIVE_TIME:
			return FT_ABSOLUTE_TIME;

		default:
			return type;
	}
}