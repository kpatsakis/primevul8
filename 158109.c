_PyAST_GetDocString(asdl_seq *body)
{
    if (!asdl_seq_LEN(body)) {
        return NULL;
    }
    stmt_ty st = (stmt_ty)asdl_seq_GET(body, 0);
    if (st->kind != Expr_kind) {
        return NULL;
    }
    expr_ty e = st->v.Expr.value;
    if (e->kind == Constant_kind && PyUnicode_CheckExact(e->v.Constant.value)) {
        return e->v.Constant.value;
    }
    return NULL;
}