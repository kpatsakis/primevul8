ExtSlice(asdl_seq * dims, PyArena *arena)
{
    slice_ty p;
    p = (slice_ty)PyArena_Malloc(arena, sizeof(*p));
    if (!p)
        return NULL;
    p->kind = ExtSlice_kind;
    p->v.ExtSlice.dims = dims;
    return p;
}