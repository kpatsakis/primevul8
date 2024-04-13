ast_for_stmt(struct compiling *c, const node *n)
{
    if (TYPE(n) == stmt) {
        assert(NCH(n) == 1);
        n = CHILD(n, 0);
    }
    if (TYPE(n) == simple_stmt) {
        assert(num_stmts(n) == 1);
        n = CHILD(n, 0);
    }
    if (TYPE(n) == small_stmt) {
        n = CHILD(n, 0);
        /* small_stmt: expr_stmt | del_stmt | pass_stmt | flow_stmt
                  | import_stmt | global_stmt | nonlocal_stmt | assert_stmt
        */
        switch (TYPE(n)) {
            case expr_stmt:
                return ast_for_expr_stmt(c, n);
            case del_stmt:
                return ast_for_del_stmt(c, n);
            case pass_stmt:
                return Pass(LINENO(n), n->n_col_offset,
                            n->n_end_lineno, n->n_end_col_offset, c->c_arena);
            case flow_stmt:
                return ast_for_flow_stmt(c, n);
            case import_stmt:
                return ast_for_import_stmt(c, n);
            case global_stmt:
                return ast_for_global_stmt(c, n);
            case nonlocal_stmt:
                return ast_for_nonlocal_stmt(c, n);
            case assert_stmt:
                return ast_for_assert_stmt(c, n);
            default:
                PyErr_Format(PyExc_SystemError,
                             "unhandled small_stmt: TYPE=%d NCH=%d\n",
                             TYPE(n), NCH(n));
                return NULL;
        }
    }
    else {
        /* compound_stmt: if_stmt | while_stmt | for_stmt | try_stmt
                        | funcdef | classdef | decorated | async_stmt
        */
        node *ch = CHILD(n, 0);
        REQ(n, compound_stmt);
        switch (TYPE(ch)) {
            case if_stmt:
                return ast_for_if_stmt(c, ch);
            case while_stmt:
                return ast_for_while_stmt(c, ch);
            case for_stmt:
                return ast_for_for_stmt(c, ch, 0);
            case try_stmt:
                return ast_for_try_stmt(c, ch);
            case with_stmt:
                return ast_for_with_stmt(c, ch, 0);
            case funcdef:
                return ast_for_funcdef(c, ch, NULL);
            case classdef:
                return ast_for_classdef(c, ch, NULL);
            case decorated:
                return ast_for_decorated(c, ch);
            case async_stmt:
                return ast_for_async_stmt(c, ch);
            default:
                PyErr_Format(PyExc_SystemError,
                             "unhandled compound_stmt: TYPE=%d NCH=%d\n",
                             TYPE(n), NCH(n));
                return NULL;
        }
    }
}