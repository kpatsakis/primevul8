future_hack(parser_state *ps)
{
    node *n = ps->p_stack.s_top->s_parent;
    node *ch, *cch;
    int i;

    /* from __future__ import ..., must have at least 4 children */
    n = CHILD(n, 0);
    if (NCH(n) < 4)
        return;
    ch = CHILD(n, 0);
    if (STR(ch) == NULL || strcmp(STR(ch), "from") != 0)
        return;
    ch = CHILD(n, 1);
    if (NCH(ch) == 1 && STR(CHILD(ch, 0)) &&
        strcmp(STR(CHILD(ch, 0)), "__future__") != 0)
        return;
    ch = CHILD(n, 3);
    /* ch can be a star, a parenthesis or import_as_names */
    if (TYPE(ch) == STAR)
        return;
    if (TYPE(ch) == LPAR)
        ch = CHILD(n, 4);

    for (i = 0; i < NCH(ch); i += 2) {
        cch = CHILD(ch, i);
        if (NCH(cch) >= 1 && TYPE(CHILD(cch, 0)) == NAME) {
            char *str_ch = STR(CHILD(cch, 0));
            if (strcmp(str_ch, FUTURE_WITH_STATEMENT) == 0) {
                ps->p_flags |= CO_FUTURE_WITH_STATEMENT;
            } else if (strcmp(str_ch, FUTURE_PRINT_FUNCTION) == 0) {
                ps->p_flags |= CO_FUTURE_PRINT_FUNCTION;
            } else if (strcmp(str_ch, FUTURE_UNICODE_LITERALS) == 0) {
                ps->p_flags |= CO_FUTURE_UNICODE_LITERALS;
            }
        }
    }
}