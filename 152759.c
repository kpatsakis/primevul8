ast_for_funcdef(struct compiling *c, const node *n, asdl_seq *decorator_seq)
{
    /* funcdef: 'def' NAME parameters ':' [TYPE_COMMENT] suite */
    identifier name;
    arguments_ty args;
    asdl_seq *body;
    int name_i = 1;
    node *tc;
    string type_comment = NULL;

    REQ(n, funcdef);

    name = NEW_IDENTIFIER(CHILD(n, name_i));
    if (!name)
        return NULL;
    else if (!forbidden_check(c, CHILD(n, name_i), STR(CHILD(n, name_i))))
        return NULL;
    args = ast_for_arguments(c, CHILD(n, name_i + 1));
    if (!args)
        return NULL;
    if (TYPE(CHILD(n, name_i + 3)) == TYPE_COMMENT) {
        type_comment = NEW_TYPE_COMMENT(CHILD(n, name_i + 3));
        name_i += 1;
    }
    body = ast_for_suite(c, CHILD(n, name_i + 3));
    if (!body)
        return NULL;

    if (!type_comment && NCH(CHILD(n, name_i + 3)) > 1) {
        /* If the function doesn't have a type comment on the same line, check
         * if the suite has a type comment in it. */
        tc = CHILD(CHILD(n, name_i + 3), 1);

        if (TYPE(tc) == TYPE_COMMENT)
            type_comment = NEW_TYPE_COMMENT(tc);
    }

    return FunctionDef(name, args, body, decorator_seq, type_comment, LINENO(n),
                       n->n_col_offset, c->c_arena);
}