correct_table_matrix(struct table *t, int col, int cspan, int a, double b)
{
    int i, j;
    int ecol = col + cspan;
    double w = 1. / (b * b);

    for (i = col; i < ecol; i++) {
	v_add_val(t->vector, i, w * a);
	for (j = i; j < ecol; j++) {
	    m_add_val(t->matrix, i, j, w);
	    m_set_val(t->matrix, j, i, m_entry(t->matrix, i, j));
	}
    }
    return i;
}