ast_for_dictcomp(struct compiling *c, const node *n)
{
    expr_ty key, value;
    asdl_seq *comps;

    assert(NCH(n) > 3);
    REQ(CHILD(n, 1), COLON);

    key = ast_for_expr(c, CHILD(n, 0));
    if (!key)
        return NULL;

    value = ast_for_expr(c, CHILD(n, 2));
    if (!value)
        return NULL;

    comps = ast_for_comprehension(c, CHILD(n, 3));
    if (!comps)
        return NULL;

    return DictComp(key, value, comps, LINENO(n), n->n_col_offset, c->c_arena);
}