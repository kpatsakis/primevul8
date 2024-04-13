ast_for_dotted_name(struct compiling *c, const node *n)
{
    expr_ty e;
    identifier id;
    int lineno, col_offset;
    int i;
    node *ch;

    REQ(n, dotted_name);

    lineno = LINENO(n);
    col_offset = n->n_col_offset;

    ch = CHILD(n, 0);
    id = NEW_IDENTIFIER(ch);
    if (!id)
        return NULL;
    e = Name(id, Load, lineno, col_offset,
             ch->n_end_lineno, ch->n_end_col_offset, c->c_arena);
    if (!e)
        return NULL;

    for (i = 2; i < NCH(n); i+=2) {
        id = NEW_IDENTIFIER(CHILD(n, i));
        if (!id)
            return NULL;
        e = Attribute(e, id, Load, lineno, col_offset,
                      n->n_end_lineno, n->n_end_col_offset, c->c_arena);
        if (!e)
            return NULL;
    }

    return e;
}