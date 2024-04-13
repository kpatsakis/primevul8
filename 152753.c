TypeIgnore(int lineno, PyArena *arena)
{
    type_ignore_ty p;
    p = (type_ignore_ty)PyArena_Malloc(arena, sizeof(*p));
    if (!p)
        return NULL;
    p->kind = TypeIgnore_kind;
    p->v.TypeIgnore.lineno = lineno;
    return p;
}