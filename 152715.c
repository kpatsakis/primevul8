seq_for_testlist(struct compiling *c, const node *n)
{
    /* testlist: test (',' test)* [','] */
    asdl_seq *seq;
    expr_ty expression;
    int i;
    assert(TYPE(n) == testlist ||
           TYPE(n) == listmaker ||
           TYPE(n) == testlist_comp ||
           TYPE(n) == testlist_safe ||
           TYPE(n) == testlist1);

    seq = asdl_seq_new((NCH(n) + 1) / 2, c->c_arena);
    if (!seq)
        return NULL;

    for (i = 0; i < NCH(n); i += 2) {
        assert(TYPE(CHILD(n, i)) == test || TYPE(CHILD(n, i)) == old_test);

        expression = ast_for_expr(c, CHILD(n, i));
        if (!expression)
            return NULL;

        assert(i / 2 < seq->size);
        asdl_seq_SET(seq, i / 2, expression);
    }
    return seq;
}