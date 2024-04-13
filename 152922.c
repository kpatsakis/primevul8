ast_for_class_bases(struct compiling *c, const node* n)
{
    /* testlist: test (',' test)* [','] */
    assert(NCH(n) > 0);
    REQ(n, testlist);
    if (NCH(n) == 1) {
        expr_ty base;
        asdl_seq *bases = asdl_seq_new(1, c->c_arena);
        if (!bases)
            return NULL;
        base = ast_for_expr(c, CHILD(n, 0));
        if (!base)
            return NULL;
        asdl_seq_SET(bases, 0, base);
        return bases;
    }

    return seq_for_testlist(c, n);
}