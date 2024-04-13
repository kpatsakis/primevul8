BinOp(expr_ty left, operator_ty op, expr_ty right, int lineno, int col_offset,
      int end_lineno, int end_col_offset, PyArena *arena)
{
    expr_ty p;
    if (!left) {
        PyErr_SetString(PyExc_ValueError,
                        "field left is required for BinOp");
        return NULL;
    }
    if (!op) {
        PyErr_SetString(PyExc_ValueError,
                        "field op is required for BinOp");
        return NULL;
    }
    if (!right) {
        PyErr_SetString(PyExc_ValueError,
                        "field right is required for BinOp");
        return NULL;
    }
    p = (expr_ty)PyArena_Malloc(arena, sizeof(*p));
    if (!p)
        return NULL;
    p->kind = BinOp_kind;
    p->v.BinOp.left = left;
    p->v.BinOp.op = op;
    p->v.BinOp.right = right;
    p->lineno = lineno;
    p->col_offset = col_offset;
    p->end_lineno = end_lineno;
    p->end_col_offset = end_col_offset;
    return p;
}