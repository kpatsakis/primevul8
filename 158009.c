copy_location(expr_ty e, const node *n)
{
    if (e) {
        e->lineno = LINENO(n);
        e->col_offset = n->n_col_offset;
        e->end_lineno = n->n_end_lineno;
        e->end_col_offset = n->n_end_col_offset;
    }
    return e;
}