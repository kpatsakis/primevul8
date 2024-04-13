hfinfo_format_text(wmem_allocator_t *scope, const header_field_info *hfinfo,
    const guchar *string)
{
	switch (hfinfo->display) {
		case STR_ASCII:
			return format_text(scope, string, strlen(string));
/*
		case STR_ASCII_WSP
			return format_text_wsp(string, strlen(string));
 */
		case STR_UNICODE:
			return format_text(scope, string, strlen(string));
	}

	return format_text(scope, string, strlen(string));
}