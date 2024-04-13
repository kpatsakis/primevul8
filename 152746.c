Module(asdl_seq * body, asdl_seq * type_ignores, PyArena *arena)
{
    mod_ty p;
    p = (mod_ty)PyArena_Malloc(arena, sizeof(*p));
    if (!p)
        return NULL;
    p->kind = Module_kind;
    p->v.Module.body = body;
    p->v.Module.type_ignores = type_ignores;
    return p;
}