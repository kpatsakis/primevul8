set_str_a_characters_bp(struct archive_write *a, unsigned char *bp,
    int from, int to, const char *s, enum vdc vdc)
{
	int r;

	switch (vdc) {
	case VDC_STD:
		set_str(bp+from, s, to - from + 1, 0x20,
		    a_characters_map);
		r = ARCHIVE_OK;
		break;
	case VDC_LOWERCASE:
		set_str(bp+from, s, to - from + 1, 0x20,
		    a1_characters_map);
		r = ARCHIVE_OK;
		break;
	case VDC_UCS2:
	case VDC_UCS2_DIRECT:
		r = set_str_utf16be(a, bp+from, s, to - from + 1,
		    0x0020, vdc);
		break;
	default:
		r = ARCHIVE_FATAL;
	}
	return (r);
}