hfinfo_type_bitwidth(enum ftenum type)
{
	int bitwidth = 0;

	switch (type) {
		case FT_CHAR:
		case FT_UINT8:
		case FT_INT8:
			bitwidth = 8;
			break;
		case FT_UINT16:
		case FT_INT16:
			bitwidth = 16;
			break;
		case FT_UINT24:
		case FT_INT24:
			bitwidth = 24;
			break;
		case FT_UINT32:
		case FT_INT32:
			bitwidth = 32;
			break;
		case FT_UINT40:
		case FT_INT40:
			bitwidth = 40;
			break;
		case FT_UINT48:
		case FT_INT48:
			bitwidth = 48;
			break;
		case FT_UINT56:
		case FT_INT56:
			bitwidth = 56;
			break;
		case FT_UINT64:
		case FT_INT64:
			bitwidth = 64;
			break;
		default:
			DISSECTOR_ASSERT_NOT_REACHED();
			;
	}
	return bitwidth;
}