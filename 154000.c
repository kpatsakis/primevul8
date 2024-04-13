correct_table_matrix2(struct table *t, int col, int cspan, double s, double b)
{
    int i, j;
    int ecol = col + cspan;
    int size = t->maxcol + 1;
    double w = 1. / (b * b);
    double ss;

    for (i = 0; i < size; i++) {
	for (j = i; j < size; j++) {
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