check_row(struct table *t, int row)
{
    int i, r;
    GeneralList ***tabdata;
    table_attr **tabattr;
    short *tabheight;
#ifdef ID_EXT
    Str **tabidvalue;
    Str *tridvalue;
#endif				/* ID_EXT */

    if (row >= t->max_rowsize) {
	r = max(t->max_rowsize * 2, row + 1);
	tabdata = New_N(GeneralList **, r);
	tabattr = New_N(table_attr *, r);
	tabheight = NewAtom_N(short, r);
#ifdef ID_EXT
	tabidvalue = New_N(Str *, r);
	tridvalue = New_N(Str, r);
#endif				/* ID_EXT */
	for (i = 0; i < t->max_rowsize; i++) {
	    tabdata[i] = t->tabdata[i];
	    tabattr[i] = t->tabattr[i];
	    tabheight[i] = t->tabheight[i];
#ifdef ID_EXT
	    tabidvalue[i] = t->tabidvalue[i];
	    tridvalue[i] = t->tridvalue[i];
#endif				/* ID_EXT */
	}
	for (; i < r; i++) {
	    tabdata[i] = NULL;
	    tabattr[i] = NULL;
	    tabheight[i] = 0;
#ifdef ID_EXT
	    tabidvalue[i] = NULL;
	    tridvalue[i] = NULL;
#endif				/* ID_EXT */
	}
	t->tabdata = tabdata;
	t->tabattr = tabattr;
	t->tabheight = tabheight;
#ifdef ID_EXT
	t->tabidvalue = tabidvalue;
	t->tridvalue = tridvalue;
#endif				/* ID_EXT */
	t->max_rowsize = r;
    }

    if (t->tabdata[row] == NULL) {
	t->tabdata[row] = New_N(GeneralList *, MAXCOL);
	t->tabattr[row] = NewAtom_N(table_attr, MAXCOL);
#ifdef ID_EXT
	t->tabidvalue[row] = New_N(Str, MAXCOL);
#endif				/* ID_EXT */
	for (i = 0; i < MAXCOL; i++) {
	    t->tabdata[row][i] = NULL;
	    t->tabattr[row][i] = 0;
#ifdef ID_EXT
	    t->tabidvalue[row][i] = NULL;
#endif				/* ID_EXT */
	}
    }
}