FunctionType(asdl_seq * argtypes, expr_ty returns, PyArena *arena)
{
    mod_ty p;
    if (!returns) {
        PyErr_SetString(PyExc_ValueError,
                        "field returns is required for FunctionType");
        return NULL;
    }
    p = (mod_ty)PyArena_Malloc(arena, sizeof(*p));
    if (!p)
        return NULL;
    p->kind = FunctionType_kind;
    p->v.FunctionType.argtypes = argtypes;
    p->v.FunctionType.returns = returns;
    return p;
}