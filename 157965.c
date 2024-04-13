ast_for_testlist(struct compiling *c, const node* n)
{
    /* testlist_comp: test (comp_for | (',' test)* [',']) */
    /* testlist: test (',' test)* [','] */
    assert(NCH(n) > 0);
    if (TYPE(n) == testlist_comp) {
        if (NCH(n) > 1)
            assert(TYPE(CHILD(n, 1)) != comp_for);
    }
    else {
        assert(TYPE(n) == testlist ||
               TYPE(n) == testlist_star_expr);
    }
    if (NCH(n) == 1)
        return ast_for_expr(c, CHILD(n, 0));
    else {
        asdl_seq *tmp = seq_for_testlist(c, n);
        if (!tmp)
            return NULL;
        return Tuple(tmp, Load, LINENO(n), n->n_col_offset,
                     n->n_end_lineno, n->n_end_col_offset, c->c_arena);
    }
}