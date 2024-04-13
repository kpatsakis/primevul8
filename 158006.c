FstringParser_Finish(FstringParser *state, struct compiling *c,
                     const node *n)
{
    asdl_seq *seq;

    FstringParser_check_invariants(state);

    /* If we're just a constant string with no expressions, return
       that. */
    if (!state->fmode) {
        assert(!state->expr_list.size);
        if (!state->last_str) {
            /* Create a zero length string. */
            state->last_str = PyUnicode_FromStringAndSize(NULL, 0);
            if (!state->last_str)
                goto error;
        }
        return make_str_node_and_del(&state->last_str, c, n);
    }

    /* Create a Constant node out of last_str, if needed. It will be the
       last node in our expression list. */
    if (state->last_str) {
        expr_ty str = make_str_node_and_del(&state->last_str, c, n);
        if (!str || ExprList_Append(&state->expr_list, str) < 0)
            goto error;
    }
    /* This has already been freed. */
    assert(state->last_str == NULL);

    seq = ExprList_Finish(&state->expr_list, c->c_arena);
    if (!seq)
        goto error;

    return JoinedStr(seq, LINENO(n), n->n_col_offset,
                     n->n_end_lineno, n->n_end_col_offset, c->c_arena);

error:
    FstringParser_Dealloc(state);
    return NULL;
}