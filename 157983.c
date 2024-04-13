ast_for_dictdisplay(struct compiling *c, const node *n)
{
    int i;
    int j;
    int size;
    asdl_seq *keys, *values;

    size = (NCH(n) + 1) / 3; /* +1 in case no trailing comma */
    keys = _Py_asdl_seq_new(size, c->c_arena);
    if (!keys)
        return NULL;

    values = _Py_asdl_seq_new(size, c->c_arena);
    if (!values)
        return NULL;

    j = 0;
    for (i = 0; i < NCH(n); i++) {
        expr_ty key, value;

        if (!ast_for_dictelement(c, n, &i, &key, &value))
            return NULL;
        asdl_seq_SET(keys, j, key);
        asdl_seq_SET(values, j, value);

        j++;
    }
    keys->size = j;
    values->size = j;
    return Dict(keys, values, LINENO(n), n->n_col_offset,
                n->n_end_lineno, n->n_end_col_offset, c->c_arena);
}