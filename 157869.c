FstringParser_ConcatFstring(FstringParser *state, const char **str,
                            const char *end, int raw, int recurse_lvl,
                            struct compiling *c, const node *n)
{
    FstringParser_check_invariants(state);
    state->fmode = 1;

    /* Parse the f-string. */
    while (1) {
        PyObject *literal = NULL;
        expr_ty expression = NULL;

        /* If there's a zero length literal in front of the
           expression, literal will be NULL. If we're at the end of
           the f-string, expression will be NULL (unless result == 1,
           see below). */
        int result = fstring_find_literal_and_expr(str, end, raw, recurse_lvl,
                                                   &literal, &expression,
                                                   c, n);
        if (result < 0)
            return -1;

        /* Add the literal, if any. */
        if (!literal) {
            /* Do nothing. Just leave last_str alone (and possibly
               NULL). */
        } else if (!state->last_str) {
            /*  Note that the literal can be zero length, if the
                input string is "\\\n" or "\\\r", among others. */
            state->last_str = literal;
            literal = NULL;
        } else {
            /* We have a literal, concatenate it. */
            assert(PyUnicode_GET_LENGTH(literal) != 0);
            if (FstringParser_ConcatAndDel(state, literal) < 0)
                return -1;
            literal = NULL;
        }

        /* We've dealt with the literal now. It can't be leaked on further
           errors. */
        assert(literal == NULL);

        /* See if we should just loop around to get the next literal
           and expression, while ignoring the expression this
           time. This is used for un-doubling braces, as an
           optimization. */
        if (result == 1)
            continue;

        if (!expression)
            /* We're done with this f-string. */
            break;

        /* We know we have an expression. Convert any existing string
           to a Constant node. */
        if (!state->last_str) {
            /* Do nothing. No previous literal. */
        } else {
            /* Convert the existing last_str literal to a Constant node. */
            expr_ty str = make_str_node_and_del(&state->last_str, c, n);
            if (!str || ExprList_Append(&state->expr_list, str) < 0)
                return -1;
        }

        if (ExprList_Append(&state->expr_list, expression) < 0)
            return -1;
    }

    /* If recurse_lvl is zero, then we must be at the end of the
       string. Otherwise, we must be at a right brace. */

    if (recurse_lvl == 0 && *str < end-1) {
        ast_error(c, n, "f-string: unexpected end of string");
        return -1;
    }
    if (recurse_lvl != 0 && **str != '}') {
        ast_error(c, n, "f-string: expecting '}'");
        return -1;
    }

    FstringParser_check_invariants(state);
    return 0;
}