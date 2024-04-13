FunctionDef(identifier name, arguments_ty args, asdl_seq * body, asdl_seq *
            decorator_list, expr_ty returns, string type_comment, int lineno,
            int col_offset, PyArena *arena)
{
    stmt_ty p;
    if (!name) {
        PyErr_SetString(PyExc_ValueError,
                        "field name is required for FunctionDef");
        return NULL;
    }
    if (!args) {
        PyErr_SetString(PyExc_ValueError,
                        "field args is required for FunctionDef");
        return NULL;
    }
    p = (stmt_ty)PyArena_Malloc(arena, sizeof(*p));
    if (!p)
        return NULL;
    p->kind = FunctionDef_kind;
    p->v.FunctionDef.name = name;
    p->v.FunctionDef.args = args;
    p->v.FunctionDef.body = body;
    p->v.FunctionDef.decorator_list = decorator_list;
    p->v.FunctionDef.returns = returns;
    p->v.FunctionDef.type_comment = type_comment;
    p->lineno = lineno;
    p->col_offset = col_offset;
    return p;
}