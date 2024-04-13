validate_exprs(asdl_seq *exprs, expr_context_ty ctx, int null_ok)
{
    Py_ssize_t i;
    for (i = 0; i < asdl_seq_LEN(exprs); i++) {
        expr_ty expr = asdl_seq_GET(exprs, i);
        if (expr) {
            if (!validate_expr(expr, ctx))
                return 0;
        }
        else if (!null_ok) {
            PyErr_SetString(PyExc_ValueError,
                            "None disallowed in expression list");
            return 0;
        }

    }
    return 1;
}