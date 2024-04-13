set_table_matrix(struct table *t, int width)
{
    int size = t->maxcol + 1;
    int i, j;
    double b, s;
    int a;
    struct table_cell *cell = &t->cell;

    if (size < 1)
	return;

    t->matrix = m_get(size, size);
    t->vector = v_get(size);
    for (i = 0; i < size; i++) {
	for (j = i; j < size; j++)
	    m_set_val(t->matrix, i, j, 0.);
	v_set_val(t->vector, i, 0.);
    }

    check_relative_width(t, width);

    for (i = 0; i < size; i++) {
	if (t->fixed_width[i] > 0) {
	    a = max(t->fixed_width[i], t->minimum_width[i]);
	    b = sigma_td(a);
	    correct_table_matrix(t, i, 1, a, b);
	}
	else if (t->fixed_width[i] < 0) {
	    s = -(double)t->fixed_width[i] / 100.;
	    b = sigma_td((int)(s * width));
	    correct_table_matrix2(t, i, 1, s, b);
	}
    }

    for (j = 0; j <= cell->maxcell; j++) {
	if (cell->fixed_width[j] > 0) {
	    a = max(cell->fixed_width[j], cell->minimum_width[j]);
	    b = sigma_td(a);
	    correct_table_matrix(t, cell->col[j], cell->colspan[j], a, b);
	}
	else if (cell->fixed_width[j] < 0) {
	    s = -(double)cell->fixed_width[j] / 100.;
	    b = sigma_td((int)(s * width));
	    correct_table_matrix2(t, cell->col[j], cell->colspan[j], s, b);
	}
    }

    set_table_matrix0(t, width);

    if (t->total_width > 0) {
	b = sigma_table(width);
    }
    else {
	b = sigma_table_nw(width);
    }
    correct_table_matrix(t, 0, size, width, b);
}