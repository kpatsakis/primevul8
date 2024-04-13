UnaryOp(unaryop_ty op, expr_ty operand, int lineno, int col_offset, PyArena
        *arena)
{
    expr_ty p;
    if (!op) {
        PyErr_SetString(PyExc_ValueError,
                        "field op is required for UnaryOp");
        return NULL;
    }
    if (!operand) {
        PyErr_SetString(PyExc_ValueError,
                        "field operand is required for UnaryOp");
        return NULL;
    }
    p = (expr_ty)PyArena_Malloc(arena, sizeof(*p));
    if (!p)
        return NULL;
    p->kind = UnaryOp_kind;
    p->v.UnaryOp.op = op;
    p->v.UnaryOp.operand = operand;
    p->lineno = lineno;
    p->col_offset = col_offset;
    return p;
}