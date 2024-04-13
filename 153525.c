process_hr(struct parsed_tag *tag, int width, int indent_width)
{
    Str tmp = Strnew_charp("<nobr>");
    int w = 0;
    int x = ALIGN_CENTER;
#define HR_ATTR_WIDTH_MAX 65535

    if (width > indent_width)
	width -= indent_width;
    if (parsedtag_get_value(tag, ATTR_WIDTH, &w)) {
	if (w > HR_ATTR_WIDTH_MAX) {
	    w = HR_ATTR_WIDTH_MAX;
	}
	w = REAL_WIDTH(w, width);
    } else {
	w = width;
    }

    parsedtag_get_value(tag, ATTR_ALIGN, &x);
    switch (x) {
    case ALIGN_CENTER:
	Strcat_charp(tmp, "<div_int align=center>");
	break;
    case ALIGN_RIGHT:
	Strcat_charp(tmp, "<div_int align=right>");
	break;
    case ALIGN_LEFT:
	Strcat_charp(tmp, "<div_int align=left>");
	break;
    }
    w /= symbol_width;
    if (w <= 0)
	w = 1;
    push_symbol(tmp, HR_SYMBOL, symbol_width, w);
    Strcat_charp(tmp, "</div_int></nobr>");
    return tmp;
}