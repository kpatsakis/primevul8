correct_table_matrix3(struct table *t, int col, char *flags, double s,
		      double b)
{
    int i, j;
    double ss;
    int size = t->maxcol + 1;
    double w = 1. / (b * b);
    int flg = (flags[col] == 0);

    for (i = 0; i < size; i++) {
	if (!((flg && flags[i] == 0) || (!flg && flags[i] != 0)))
	    continue;
	for (j = i; j < size; j++) {
	    if (!((flg && flags[j] == 0) || (!flg && flags[j] != 0)))
		continue;
	    if (i == col && j == col)
		ss = (1. - s) * (1. - s);
	    else if (i == col || j == col)
		ss = -(1. - s) * s;
	    else
		ss = s * s;
	    m_add_val(t->matrix, i, j, w * ss);
	}
    }
}