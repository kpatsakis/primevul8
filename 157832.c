BoolOp(boolop_ty op, asdl_seq * values, int lineno, int col_offset, int
       end_lineno, int end_col_offset, PyArena *arena)
{
    expr_ty p;
    if (!op) {
        PyErr_SetString(PyExc_ValueError,
                        "field op is required for BoolOp");
        return NULL;
    }
    p = (expr_ty)PyArena_Malloc(arena, sizeof(*p));
    if (!p)
        return NULL;
    p->kind = BoolOp_kind;
    p->v.BoolOp.op = op;
    p->v.BoolOp.values = values;
    p->lineno = lineno;
    p->col_offset = col_offset;
    p->end_lineno = end_lineno;
    p->end_col_offset = end_col_offset;
    return p;
}