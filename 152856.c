GeneratorExp(expr_ty elt, asdl_seq * generators, int lineno, int col_offset,
             PyArena *arena)
{
    expr_ty p;
    if (!elt) {
        PyErr_SetString(PyExc_ValueError,
                        "field elt is required for GeneratorExp");
        return NULL;
    }
    p = (expr_ty)PyArena_Malloc(arena, sizeof(*p));
    if (!p)
        return NULL;
    p->kind = GeneratorExp_kind;
    p->v.GeneratorExp.elt = elt;
    p->v.GeneratorExp.generators = generators;
    p->lineno = lineno;
    p->col_offset = col_offset;
    return p;
}