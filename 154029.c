correct_table_matrix4(struct table *t, int col, int cspan, char *flags,
		      double s, double b)
{
    int i, j;
    double ss;
    int ecol = col + cspan;
    int size = t->maxcol + 1;
    double w = 1. / (b * b);

    for (i = 0; i < size; i++) {
	if (flags[i] && !(i >= col && i < ecol))
	    continue;
	for (j = i; j < size; j++) {
	    if (flags[j] && !(j >= col && j < ecol))
		continue;
	    if (i >= col && i < ecol && j >= col && j < ecol)
		ss = (1. - s) * (1. - s);
	    else if ((i >= col && i < ecol) || (j >= col && j < ecol))
		ss = -(1. - s) * s;
	    else
		ss = s * s;
	    m_add_val(t->matrix, i, j, w * ss);
	}
    }
}