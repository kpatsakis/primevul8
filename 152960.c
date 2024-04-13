withitem(expr_ty context_expr, expr_ty optional_vars, PyArena *arena)
{
    withitem_ty p;
    if (!context_expr) {
        PyErr_SetString(PyExc_ValueError,
                        "field context_expr is required for withitem");
        return NULL;
    }
    p = (withitem_ty)PyArena_Malloc(arena, sizeof(*p));
    if (!p)
        return NULL;
    p->context_expr = context_expr;
    p->optional_vars = optional_vars;
    return p;
}