newTable()
{
    struct table *t;
    int i, j;

    t = New(struct table);
    t->max_rowsize = MAXROW;
    t->tabdata = New_N(GeneralList **, MAXROW);
    t->tabattr = New_N(table_attr *, MAXROW);
    t->tabheight = NewAtom_N(short, MAXROW);
#ifdef ID_EXT
    t->tabidvalue = New_N(Str *, MAXROW);
    t->tridvalue = New_N(Str, MAXROW);
#endif				/* ID_EXT */

    for (i = 0; i < MAXROW; i++) {
	t->tabdata[i] = NULL;
	t->tabattr[i] = 0;
	t->tabheight[i] = 0;
#ifdef ID_EXT
	t->tabidvalue[i] = NULL;
	t->tridvalue[i] = NULL;
#endif				/* ID_EXT */
    }
    for (j = 0; j < MAXCOL; j++) {
	t->tabwidth[j] = 0;
	t->minimum_width[j] = 0;
	t->fixed_width[j] = 0;
    }
    t->cell.maxcell = -1;
    t->cell.icell = -1;
    t->ntable = 0;
    t->tables_size = 0;
    t->tables = NULL;
#ifdef MATRIX
    t->matrix = NULL;
    t->vector = NULL;
#endif				/* MATRIX */
#if 0
    t->tabcontentssize = 0;
    t->indent = 0;
    t->linfo.prev_ctype = PC_ASCII;
    t->linfo.prev_spaces = -1;
#endif
    t->linfo.prevchar = Strnew_size(8);
    set_prevchar(t->linfo.prevchar, "", 0);
    t->trattr = 0;

    t->caption = Strnew();
    t->suspended_data = NULL;
#ifdef ID_EXT
    t->id = NULL;
#endif
    return t;
}