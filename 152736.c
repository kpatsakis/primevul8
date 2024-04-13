ast_for_decorators(struct compiling *c, const node *n)
{
    asdl_seq* decorator_seq;
    expr_ty d;
    int i;

    REQ(n, decorators);
    decorator_seq = asdl_seq_new(NCH(n), c->c_arena);
    if (!decorator_seq)
        return NULL;

    for (i = 0; i < NCH(n); i++) {
        d = ast_for_decorator(c, CHILD(n, i));
        if (!d)
            return NULL;
        asdl_seq_SET(decorator_seq, i, d);
    }
    return decorator_seq;
}