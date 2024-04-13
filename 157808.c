Compare(expr_ty left, asdl_int_seq * ops, asdl_seq * comparators, int lineno,
        int col_offset, int end_lineno, int end_col_offset, PyArena *arena)
{
    expr_ty p;
    if (!left) {
        PyErr_SetString(PyExc_ValueError,
                        "field left is required for Compare");
        return NULL;
    }
    p = (expr_ty)PyArena_Malloc(arena, sizeof(*p));
    if (!p)
        return NULL;
    p->kind = Compare_kind;
    p->v.Compare.left = left;
    p->v.Compare.ops = ops;
    p->v.Compare.comparators = comparators;
    p->lineno = lineno;
    p->col_offset = col_offset;
    p->end_lineno = end_lineno;
    p->end_col_offset = end_col_offset;
    return p;
}