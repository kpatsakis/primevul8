ast_for_dictcomp(struct compiling *c, const node *n)
{
    expr_ty key, value;
    asdl_seq *comps;
    int i = 0;

    if (!ast_for_dictelement(c, n, &i, &key, &value))
        return NULL;
    assert(key);
    assert(NCH(n) - i >= 1);

    comps = ast_for_comprehension(c, CHILD(n, i));
    if (!comps)
        return NULL;

    return DictComp(key, value, comps, LINENO(n), n->n_col_offset,
                    n->n_end_lineno, n->n_end_col_offset, c->c_arena);
}