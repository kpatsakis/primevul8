ast_for_flow_stmt(struct compiling *c, const node *n)
{
    /*
      flow_stmt: break_stmt | continue_stmt | return_stmt | raise_stmt
                 | yield_stmt
      break_stmt: 'break'
      continue_stmt: 'continue'
      return_stmt: 'return' [testlist]
      yield_stmt: yield_expr
      yield_expr: 'yield' testlist | 'yield' 'from' test
      raise_stmt: 'raise' [test [',' test [',' test]]]
    */
    node *ch;

    REQ(n, flow_stmt);
    ch = CHILD(n, 0);
    switch (TYPE(ch)) {
        case break_stmt:
            return Break(LINENO(n), n->n_col_offset,
                         n->n_end_lineno, n->n_end_col_offset, c->c_arena);
        case continue_stmt:
            return Continue(LINENO(n), n->n_col_offset,
                            n->n_end_lineno, n->n_end_col_offset, c->c_arena);
        case yield_stmt: { /* will reduce to yield_expr */
            expr_ty exp = ast_for_expr(c, CHILD(ch, 0));
            if (!exp)
                return NULL;
            return Expr(exp, LINENO(n), n->n_col_offset,
                        n->n_end_lineno, n->n_end_col_offset, c->c_arena);
        }
        case return_stmt:
            if (NCH(ch) == 1)
                return Return(NULL, LINENO(n), n->n_col_offset,
                              n->n_end_lineno, n->n_end_col_offset, c->c_arena);
            else {
                expr_ty expression = ast_for_testlist(c, CHILD(ch, 1));
                if (!expression)
                    return NULL;
                return Return(expression, LINENO(n), n->n_col_offset,
                              n->n_end_lineno, n->n_end_col_offset, c->c_arena);
            }
        case raise_stmt:
            if (NCH(ch) == 1)
                return Raise(NULL, NULL, LINENO(n), n->n_col_offset,
                             n->n_end_lineno, n->n_end_col_offset, c->c_arena);
            else if (NCH(ch) >= 2) {
                expr_ty cause = NULL;
                expr_ty expression = ast_for_expr(c, CHILD(ch, 1));
                if (!expression)
                    return NULL;
                if (NCH(ch) == 4) {
                    cause = ast_for_expr(c, CHILD(ch, 3));
                    if (!cause)
                        return NULL;
                }
                return Raise(expression, cause, LINENO(n), n->n_col_offset,
                             n->n_end_lineno, n->n_end_col_offset, c->c_arena);
            }
            /* fall through */
        default:
            PyErr_Format(PyExc_SystemError,
                         "unexpected flow_stmt: %d", TYPE(ch));
            return NULL;
    }
}