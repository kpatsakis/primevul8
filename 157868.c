ast_for_nonlocal_stmt(struct compiling *c, const node *n)
{
    /* nonlocal_stmt: 'nonlocal' NAME (',' NAME)* */
    identifier name;
    asdl_seq *s;
    int i;

    REQ(n, nonlocal_stmt);
    s = _Py_asdl_seq_new(NCH(n) / 2, c->c_arena);
    if (!s)
        return NULL;
    for (i = 1; i < NCH(n); i += 2) {
        name = NEW_IDENTIFIER(CHILD(n, i));
        if (!name)
            return NULL;
        asdl_seq_SET(s, i / 2, name);
    }
    return Nonlocal(s, LINENO(n), n->n_col_offset,
                    n->n_end_lineno, n->n_end_col_offset, c->c_arena);
}