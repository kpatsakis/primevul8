align(TextLine *lbuf, int width, int mode)
{
    int i, l, l1, l2;
    Str buf, line = lbuf->line;

    if (line->length == 0) {
	for (i = 0; i < width; i++)
	    Strcat_char(line, ' ');
	lbuf->pos = width;
	return;
    }
    buf = Strnew();
    l = width - lbuf->pos;
    switch (mode) {
    case ALIGN_CENTER:
	l1 = l / 2;
	l2 = l - l1;
	for (i = 0; i < l1; i++)
	    Strcat_char(buf, ' ');
	Strcat(buf, line);
	for (i = 0; i < l2; i++)
	    Strcat_char(buf, ' ');
	break;
    case ALIGN_LEFT:
	Strcat(buf, line);
	for (i = 0; i < l; i++)
	    Strcat_char(buf, ' ');
	break;
    case ALIGN_RIGHT:
	for (i = 0; i < l; i++)
	    Strcat_char(buf, ' ');
	Strcat(buf, line);
	break;
    default:
	return;
    }
    lbuf->line = buf;
    if (lbuf->pos < width)
	lbuf->pos = width;
}