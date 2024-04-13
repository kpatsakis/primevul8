ast_for_flow_stmt(struct compiling *c, const node *n)
{
    /*
      flow_stmt: break_stmt | continue_stmt | return_stmt | raise_stmt
                 | yield_stmt
      break_stmt: 'break'
      continue_stmt: 'continue'
      return_stmt: 'return' [testlist]
      yield_stmt: yield_expr
      yield_expr: 'yield' testlist
      raise_stmt: 'raise' [test [',' test [',' test]]]
    */
    node *ch;

    REQ(n, flow_stmt);
    ch = CHILD(n, 0);
    switch (TYPE(ch)) {
        case break_stmt:
            return Break(LINENO(n), n->n_col_offset, c->c_arena);
        case continue_stmt:
            return Continue(LINENO(n), n->n_col_offset, c->c_arena);
        case yield_stmt: { /* will reduce to yield_expr */
            expr_ty exp = ast_for_expr(c, CHILD(ch, 0));
            if (!exp)
                return NULL;
            return Expr(exp, LINENO(n), n->n_col_offset, c->c_arena);
        }
        case return_stmt:
            if (NCH(ch) == 1)
                return Return(NULL, LINENO(n), n->n_col_offset, c->c_arena);
            else {
                expr_ty expression = ast_for_testlist(c, CHILD(ch, 1));
                if (!expression)
                    return NULL;
                return Return(expression, LINENO(n), n->n_col_offset,
                              c->c_arena);
            }
        case raise_stmt:
            if (NCH(ch) == 1)
                return Raise(NULL, NULL, NULL, LINENO(n), n->n_col_offset,
                             c->c_arena);
            else if (NCH(ch) == 2) {
                expr_ty expression = ast_for_expr(c, CHILD(ch, 1));
                if (!expression)
                    return NULL;
                return Raise(expression, NULL, NULL, LINENO(n),
                             n->n_col_offset, c->c_arena);
            }
            else if (NCH(ch) == 4) {
                expr_ty expr1, expr2;

                expr1 = ast_for_expr(c, CHILD(ch, 1));
                if (!expr1)
                    return NULL;
                expr2 = ast_for_expr(c, CHILD(ch, 3));
                if (!expr2)
                    return NULL;

                return Raise(expr1, expr2, NULL, LINENO(n), n->n_col_offset,
                             c->c_arena);
            }
            else if (NCH(ch) == 6) {
                expr_ty expr1, expr2, expr3;

                expr1 = ast_for_expr(c, CHILD(ch, 1));
                if (!expr1)
                    return NULL;
                expr2 = ast_for_expr(c, CHILD(ch, 3));
                if (!expr2)
                    return NULL;
                expr3 = ast_for_expr(c, CHILD(ch, 5));
                if (!expr3)
                    return NULL;

                return Raise(expr1, expr2, expr3, LINENO(n), n->n_col_offset,
                             c->c_arena);
            }
        default:
            PyErr_Format(PyExc_SystemError,
                         "unexpected flow_stmt: %d", TYPE(ch));
            return NULL;
    }
}