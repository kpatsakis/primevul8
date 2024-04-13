ast_for_listcomp(struct compiling *c, const node *n)
{
    /* listmaker: test ( list_for | (',' test)* [','] )
       list_for: 'for' exprlist 'in' testlist_safe [list_iter]
       list_iter: list_for | list_if
       list_if: 'if' test [list_iter]
       testlist_safe: test [(',' test)+ [',']]
    */
    expr_ty elt, first;
    asdl_seq *listcomps;
    int i, n_fors;
    node *ch;

    REQ(n, listmaker);
    assert(NCH(n) > 1);

    elt = ast_for_expr(c, CHILD(n, 0));
    if (!elt)
        return NULL;

    n_fors = count_list_fors(c, n);
    if (n_fors == -1)
        return NULL;

    listcomps = asdl_seq_new(n_fors, c->c_arena);
    if (!listcomps)
        return NULL;

    ch = CHILD(n, 1);
    for (i = 0; i < n_fors; i++) {
        comprehension_ty lc;
        asdl_seq *t;
        expr_ty expression;
        node *for_ch;

        REQ(ch, list_for);

        for_ch = CHILD(ch, 1);
        t = ast_for_exprlist(c, for_ch, Store);
        if (!t)
            return NULL;
        expression = ast_for_testlist(c, CHILD(ch, 3));
        if (!expression)
            return NULL;

        /* Check the # of children rather than the length of t, since
           [x for x, in ... ] has 1 element in t, but still requires a Tuple.
        */
        first = (expr_ty)asdl_seq_GET(t, 0);
        if (NCH(for_ch) == 1)
            lc = comprehension(first, expression, NULL, c->c_arena);
        else
            lc = comprehension(Tuple(t, Store, first->lineno, first->col_offset,
                                     c->c_arena),
                               expression, NULL, c->c_arena);
        if (!lc)
            return NULL;

        if (NCH(ch) == 5) {
            int j, n_ifs;
            asdl_seq *ifs;
            expr_ty list_for_expr;

            ch = CHILD(ch, 4);
            n_ifs = count_list_ifs(c, ch);
            if (n_ifs == -1)
                return NULL;

            ifs = asdl_seq_new(n_ifs, c->c_arena);
            if (!ifs)
                return NULL;

            for (j = 0; j < n_ifs; j++) {
                REQ(ch, list_iter);
                ch = CHILD(ch, 0);
                REQ(ch, list_if);

                list_for_expr = ast_for_expr(c, CHILD(ch, 1));
                if (!list_for_expr)
                    return NULL;

                asdl_seq_SET(ifs, j, list_for_expr);
                if (NCH(ch) == 3)
                    ch = CHILD(ch, 2);
            }
            /* on exit, must guarantee that ch is a list_for */
            if (TYPE(ch) == list_iter)
                ch = CHILD(ch, 0);
            lc->ifs = ifs;
        }
        asdl_seq_SET(listcomps, i, lc);
    }

    return ListComp(elt, listcomps, LINENO(n), n->n_col_offset, c->c_arena);
}