print_item(struct table *t, int row, int col, int width, Str buf)
{
    int alignment;
    TextLine *lbuf;

    if (t->tabdata[row])
	lbuf = popTextLine(t->tabdata[row][col]);
    else
	lbuf = NULL;

    if (lbuf != NULL) {
	check_row(t, row);
	alignment = ALIGN_CENTER;
	if ((t->tabattr[row][col] & HTT_ALIGN) == HTT_LEFT)
	    alignment = ALIGN_LEFT;
	else if ((t->tabattr[row][col] & HTT_ALIGN) == HTT_RIGHT)
	    alignment = ALIGN_RIGHT;
	else if ((t->tabattr[row][col] & HTT_ALIGN) == HTT_CENTER)
	    alignment = ALIGN_CENTER;
	align(lbuf, width, alignment);
	Strcat(buf, lbuf->line);
    }
    else {
	lbuf = newTextLine(NULL, 0);
	align(lbuf, width, ALIGN_CENTER);
	Strcat(buf, lbuf->line);
    }
}