begin_table(int border, int spacing, int padding, int vspace)
{
    struct table *t;
    int mincell = minimum_cellspacing(border);
    int rcellspacing;
    int mincell_pixels = round(mincell * pixel_per_char);
    int ppc = round(pixel_per_char);

    t = newTable();
    t->row = t->col = -1;
    t->maxcol = -1;
    t->maxrow = -1;
    t->border_mode = border;
    t->flag = 0;
    if (border == BORDER_NOWIN)
	t->flag |= TBL_EXPAND_OK;

    rcellspacing = spacing + 2 * padding;
    switch (border) {
    case BORDER_THIN:
    case BORDER_THICK:
    case BORDER_NOWIN:
	t->cellpadding = padding - (mincell_pixels - 4) / 2;
	break;
    case BORDER_NONE:
	t->cellpadding = rcellspacing - mincell_pixels;
    }
    if (t->cellpadding >= ppc)
	t->cellpadding /= ppc;
    else if (t->cellpadding > 0)
	t->cellpadding = 1;
    else
	t->cellpadding = 0;

    switch (border) {
    case BORDER_THIN:
    case BORDER_THICK:
    case BORDER_NOWIN:
	t->cellspacing = 2 * t->cellpadding + mincell;
	break;
    case BORDER_NONE:
	t->cellspacing = t->cellpadding + mincell;
    }

    if (border == BORDER_NONE) {
	if (rcellspacing / 2 + vspace <= 1)
	    t->vspace = 0;
	else
	    t->vspace = 1;
    }
    else {
	if (vspace < ppc)
	    t->vspace = 0;
	else
	    t->vspace = 1;
    }

    if (border == BORDER_NONE) {
	if (rcellspacing <= THR_PADDING)
	    t->vcellpadding = 0;
	else
	    t->vcellpadding = 1;
    }
    else {
	if (padding < 2 * ppc - 2)
	    t->vcellpadding = 0;
	else
	    t->vcellpadding = 1;
    }

    return t;
}