Suite(asdl_seq * body, PyArena *arena)
{
    mod_ty p;
    p = (mod_ty)PyArena_Malloc(arena, sizeof(*p));
    if (!p)
        return NULL;
    p->kind = Suite_kind;
    p->v.Suite.body = body;
    return p;
}