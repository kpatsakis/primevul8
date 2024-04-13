seq_for_testlist(struct compiling *c, const node *n)
{
    /* testlist: test (',' test)* [',']
       testlist_star_expr: test|star_expr (',' test|star_expr)* [',']
    */
    asdl_seq *seq;
    expr_ty expression;
    int i;
    assert(TYPE(n) == testlist || TYPE(n) == testlist_star_expr || TYPE(n) == testlist_comp);

    seq = _Py_asdl_seq_new((NCH(n) + 1) / 2, c->c_arena);
    if (!seq)
        return NULL;

    for (i = 0; i < NCH(n); i += 2) {
        const node *ch = CHILD(n, i);
        assert(TYPE(ch) == test || TYPE(ch) == test_nocond || TYPE(ch) == star_expr || TYPE(ch) == namedexpr_test);

        expression = ast_for_expr(c, ch);
        if (!expression)
            return NULL;

        assert(i / 2 < seq->size);
        asdl_seq_SET(seq, i / 2, expression);
    }
    return seq;
}