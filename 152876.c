ast_for_with_stmt(struct compiling *c, const node *n)
{
    int i, has_type_comment;
    stmt_ty ret;
    asdl_seq *inner;
    string type_comment;

    REQ(n, with_stmt);

    has_type_comment = TYPE(CHILD(n, NCH(n) - 2)) == TYPE_COMMENT;

    /* process the with items inside-out */
    i = NCH(n) - 1;
    /* the suite of the innermost with item is the suite of the with stmt */
    inner = ast_for_suite(c, CHILD(n, i));
    if (!inner)
        return NULL;

    if (has_type_comment) {
        type_comment = NEW_TYPE_COMMENT(CHILD(n, NCH(n) - 2));
        i--;
    } else
        type_comment = NULL;


    for (;;) {
        i -= 2;
        ret = ast_for_with_item(c, CHILD(n, i), inner, type_comment);
        if (!ret)
            return NULL;
        /* was this the last item? */
        if (i == 1)
            break;
        /* if not, wrap the result so far in a new sequence */
        inner = asdl_seq_new(1, c->c_arena);
        if (!inner)
            return NULL;
        asdl_seq_SET(inner, 0, ret);
    }

    return ret;
}