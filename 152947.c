Interactive(asdl_seq * body, PyArena *arena)
{
    mod_ty p;
    p = (mod_ty)PyArena_Malloc(arena, sizeof(*p));
    if (!p)
        return NULL;
    p->kind = Interactive_kind;
    p->v.Interactive.body = body;
    return p;
}