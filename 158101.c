AsyncFunctionDef(identifier name, arguments_ty args, asdl_seq * body, asdl_seq
                 * decorator_list, expr_ty returns, string type_comment, int
                 lineno, int col_offset, int end_lineno, int end_col_offset,
                 PyArena *arena)
{
    stmt_ty p;
    if (!name) {
        PyErr_SetString(PyExc_ValueError,
                        "field name is required for AsyncFunctionDef");
        return NULL;
    }
    if (!args) {
        PyErr_SetString(PyExc_ValueError,
                        "field args is required for AsyncFunctionDef");
        return NULL;
    }
    p = (stmt_ty)PyArena_Malloc(arena, sizeof(*p));
    if (!p)
        return NULL;
    p->kind = AsyncFunctionDef_kind;
    p->v.AsyncFunctionDef.name = name;
    p->v.AsyncFunctionDef.args = args;
    p->v.AsyncFunctionDef.body = body;
    p->v.AsyncFunctionDef.decorator_list = decorator_list;
    p->v.AsyncFunctionDef.returns = returns;
    p->v.AsyncFunctionDef.type_comment = type_comment;
    p->lineno = lineno;
    p->col_offset = col_offset;
    p->end_lineno = end_lineno;
    p->end_col_offset = end_col_offset;
    return p;
}