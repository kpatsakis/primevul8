check_table_height(struct table *t)
{
    int i, j, k;
    struct {
	short *row;
	short *rowspan;
	short *indexarray;
	short maxcell;
	short size;
	short *height;
    } cell;
    int space = 0;

    cell.size = 0;
    cell.maxcell = -1;

    for (j = 0; j <= t->maxrow; j++) {
	if (!t->tabattr[j])
	    continue;
	for (i = 0; i <= t->maxcol; i++) {
	    int t_dep, rowspan;
	    if (t->tabattr[j][i] & (HTT_X | HTT_Y))
		continue;

	    if (t->tabdata[j][i] == NULL)
		t_dep = 0;
	    else
		t_dep = t->tabdata[j][i]->nitem;

	    rowspan = table_rowspan(t, j, i);
	    if (rowspan > 1) {
		int c = cell.maxcell + 1;
		k = bsearch_2short(rowspan, cell.rowspan,
				   j, cell.row, t->maxrow + 1, cell.indexarray,
				   c);
		if (k <= cell.maxcell) {
		    int idx = cell.indexarray[k];
		    if (cell.row[idx] == j && cell.rowspan[idx] == rowspan)
			c = idx;
		}
		if (c >= MAXROWCELL)
		    continue;
		if (c >= cell.size) {
		    if (cell.size == 0) {
			cell.size = max(MAXCELL, c + 1);
			cell.row = NewAtom_N(short, cell.size);
			cell.rowspan = NewAtom_N(short, cell.size);
			cell.indexarray = NewAtom_N(short, cell.size);
			cell.height = NewAtom_N(short, cell.size);
		    }
		    else {
			cell.size = max(cell.size + MAXCELL, c + 1);
			cell.row = New_Reuse(short, cell.row, cell.size);
			cell.rowspan = New_Reuse(short, cell.rowspan,
						 cell.size);
			cell.indexarray = New_Reuse(short, cell.indexarray,
						    cell.size);
			cell.height = New_Reuse(short, cell.height, cell.size);
		    }
		}
		if (c > cell.maxcell) {
		    cell.maxcell++;
		    cell.row[cell.maxcell] = j;
		    cell.rowspan[cell.maxcell] = rowspan;
		    cell.height[cell.maxcell] = 0;
		    if (cell.maxcell > k) {
			int ii;
			for (ii = cell.maxcell; ii > k; ii--)
			    cell.indexarray[ii] = cell.indexarray[ii - 1];
		    }
		    cell.indexarray[k] = cell.maxcell;
		}

		if (cell.height[c] < t_dep)
		    cell.height[c] = t_dep;
		continue;
	    }
	    if (t->tabheight[j] < t_dep)
		t->tabheight[j] = t_dep;
	}
    }

    switch (t->border_mode) {
    case BORDER_THIN:
    case BORDER_THICK:
    case BORDER_NOWIN:
	space = 1;
	break;
    case BORDER_NONE:
	space = 0;
    }
    check_cell_width(t->tabheight, cell.height, cell.row, cell.rowspan,
		     cell.maxcell, cell.indexarray, space, 1);
}