Ta27AST_FromNode(const node *n, PyCompilerFlags *flags, const char *filename,
               PyArena *arena)
{
    int i, j, k, num;
    asdl_seq *stmts = NULL;
    asdl_seq *type_ignores = NULL;
    stmt_ty s;
    node *ch;
    struct compiling c;
    asdl_seq *argtypes = NULL;
    expr_ty ret, arg;

    if (flags && flags->cf_flags & PyCF_SOURCE_IS_UTF8) {
        c.c_encoding = "utf-8";
        if (TYPE(n) == encoding_decl) {
            ast_error(n, "encoding declaration in Unicode string");
            goto error;
        }
    } else if (TYPE(n) == encoding_decl) {
        c.c_encoding = STR(n);
        n = CHILD(n, 0);
    } else {
        c.c_encoding = NULL;
    }
    c.c_future_unicode = flags && flags->cf_flags & CO_FUTURE_UNICODE_LITERALS;
    c.c_arena = arena;
    c.c_filename = filename;

    k = 0;
    switch (TYPE(n)) {
        case file_input:
            stmts = asdl_seq_new(num_stmts(n), arena);
            if (!stmts)
                return NULL;
            for (i = 0; i < NCH(n) - 1; i++) {
                ch = CHILD(n, i);
                if (TYPE(ch) == NEWLINE)
                    continue;
                REQ(ch, stmt);
                num = num_stmts(ch);
                if (num == 1) {
                    s = ast_for_stmt(&c, ch);
                    if (!s)
                        goto error;
                    asdl_seq_SET(stmts, k++, s);
                }
                else {
                    ch = CHILD(ch, 0);
                    REQ(ch, simple_stmt);
                    for (j = 0; j < num; j++) {
                        s = ast_for_stmt(&c, CHILD(ch, j * 2));
                        if (!s)
                            goto error;
                        asdl_seq_SET(stmts, k++, s);
                    }
                }
            }
            /* Type ignores are stored under the ENDMARKER in file_input. */
            ch = CHILD(n, NCH(n) - 1);
            REQ(ch, ENDMARKER);
            num = NCH(ch);
            type_ignores = _Py_asdl_seq_new(num, arena);
            if (!type_ignores)
                goto error;

            for (i = 0; i < num; i++) {
                type_ignore_ty ti = TypeIgnore(LINENO(CHILD(ch, i)), arena);
                if (!ti)
                    goto error;
                asdl_seq_SET(type_ignores, i, ti);
            }

            return Module(stmts, type_ignores, arena);
        case eval_input: {
            expr_ty testlist_ast;

            /* XXX Why not comp_for here? */
            testlist_ast = ast_for_testlist(&c, CHILD(n, 0));
            if (!testlist_ast)
                goto error;
            return Expression(testlist_ast, arena);
        }
        case single_input:
            if (TYPE(CHILD(n, 0)) == NEWLINE) {
                stmts = asdl_seq_new(1, arena);
                if (!stmts)
                    goto error;
                asdl_seq_SET(stmts, 0, Pass(n->n_lineno, n->n_col_offset,
                                            arena));
                if (!asdl_seq_GET(stmts, 0))
                    goto error;
                return Interactive(stmts, arena);
            }
            else {
                n = CHILD(n, 0);
                num = num_stmts(n);
                stmts = asdl_seq_new(num, arena);
                if (!stmts)
                    goto error;
                if (num == 1) {
                    s = ast_for_stmt(&c, n);
                    if (!s)
                        goto error;
                    asdl_seq_SET(stmts, 0, s);
                }
                else {
                    /* Only a simple_stmt can contain multiple statements. */
                    REQ(n, simple_stmt);
                    for (i = 0; i < NCH(n); i += 2) {
                        if (TYPE(CHILD(n, i)) == NEWLINE)
                            break;
                        s = ast_for_stmt(&c, CHILD(n, i));
                        if (!s)
                            goto error;
                        asdl_seq_SET(stmts, i / 2, s);
                    }
                }

                return Interactive(stmts, arena);
            }
        case func_type_input:
            n = CHILD(n, 0);
            REQ(n, func_type);

            if (TYPE(CHILD(n, 1)) == typelist) {
                ch = CHILD(n, 1);
                /* this is overly permissive -- we don't pay any attention to
                 * stars on the args -- just parse them into an ordered list */
                num = 0;
                for (i = 0; i < NCH(ch); i++) {
                    if (TYPE(CHILD(ch, i)) == test)
                        num++;
                }

                argtypes = _Py_asdl_seq_new(num, arena);

                j = 0;
                for (i = 0; i < NCH(ch); i++) {
                    if (TYPE(CHILD(ch, i)) == test) {
                        arg = ast_for_expr(&c, CHILD(ch, i));
                        if (!arg)
                            goto error;
                        asdl_seq_SET(argtypes, j++, arg);
                    }
                }
            }
            else
                argtypes = _Py_asdl_seq_new(0, arena);

            ret = ast_for_expr(&c, CHILD(n, NCH(n) - 1));
            if (!ret)
                goto error;
            return FunctionType(argtypes, ret, arena);
        default:
            PyErr_Format(PyExc_SystemError,
                         "invalid node %d for Ta27AST_FromNode", TYPE(n));
            goto error;
    }
 error:
    ast_error_finish(filename);
    return NULL;
}