ast_for_expr_stmt(struct compiling *c, const node *n)
{
    int num;
    REQ(n, expr_stmt);
    /* expr_stmt: testlist (augassign (yield_expr|testlist)
                | ('=' (yield_expr|testlist))* [TYPE_COMMENT])
       testlist: test (',' test)* [',']
       augassign: '+=' | '-=' | '*=' | '/=' | '%=' | '&=' | '|=' | '^='
                | '<<=' | '>>=' | '**=' | '//='
       test: ... here starts the operator precendence dance
     */
    num = NCH(n);

    if (num == 1 || (num == 2 && TYPE(CHILD(n, 1)) == TYPE_COMMENT)) {
        expr_ty e = ast_for_testlist(c, CHILD(n, 0));
        if (!e)
            return NULL;

        return Expr(e, LINENO(n), n->n_col_offset, c->c_arena);
    }
    else if (TYPE(CHILD(n, 1)) == augassign) {
        expr_ty expr1, expr2;
        operator_ty newoperator;
        node *ch = CHILD(n, 0);

        expr1 = ast_for_testlist(c, ch);
        if (!expr1)
            return NULL;
        if(!set_context(c, expr1, Store, ch))
            return NULL;
        /* set_context checks that most expressions are not the left side.
          Augmented assignments can only have a name, a subscript, or an
          attribute on the left, though, so we have to explicitly check for
          those. */
        switch (expr1->kind) {
            case Name_kind:
            case Attribute_kind:
            case Subscript_kind:
                break;
            default:
                ast_error(ch, "illegal expression for augmented assignment");
                return NULL;
        }

        ch = CHILD(n, 2);
        if (TYPE(ch) == testlist)
            expr2 = ast_for_testlist(c, ch);
        else
            expr2 = ast_for_expr(c, ch);
        if (!expr2)
            return NULL;

        newoperator = ast_for_augassign(c, CHILD(n, 1));
        if (!newoperator)
            return NULL;

        return AugAssign(expr1, newoperator, expr2, LINENO(n), n->n_col_offset,
                         c->c_arena);
    }
    else {
        int i, nch_minus_type, has_type_comment;
        asdl_seq *targets;
        node *value;
        expr_ty expression;
        string type_comment;

        /* a normal assignment */
        REQ(CHILD(n, 1), EQUAL);

        has_type_comment = TYPE(CHILD(n, num - 1)) == TYPE_COMMENT;
        nch_minus_type = num - has_type_comment;

        targets = asdl_seq_new(nch_minus_type / 2, c->c_arena);
        if (!targets)
            return NULL;
        for (i = 0; i < nch_minus_type - 2; i += 2) {
            expr_ty e;
            node *ch = CHILD(n, i);
            if (TYPE(ch) == yield_expr) {
                ast_error(ch, "assignment to yield expression not possible");
                return NULL;
            }
            e = ast_for_testlist(c, ch);
            if (!e)
                return NULL;

            /* set context to assign */
            if (!set_context(c, e, Store, CHILD(n, i)))
                return NULL;

            asdl_seq_SET(targets, i / 2, e);
        }
        value = CHILD(n, nch_minus_type - 1);
        if (TYPE(value) == testlist)
            expression = ast_for_testlist(c, value);
        else
            expression = ast_for_expr(c, value);
        if (!expression)
            return NULL;
        if (has_type_comment)
            type_comment = NEW_TYPE_COMMENT(CHILD(n, nch_minus_type));
        else
            type_comment = NULL;
        return Assign(targets, expression, type_comment, LINENO(n), n->n_col_offset,
                      c->c_arena);
    }
}