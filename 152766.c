Slice(expr_ty lower, expr_ty upper, expr_ty step, PyArena *arena)
{
    slice_ty p;
    p = (slice_ty)PyArena_Malloc(arena, sizeof(*p));
    if (!p)
        return NULL;
    p->kind = Slice_kind;
    p->v.Slice.lower = lower;
    p->v.Slice.upper = upper;
    p->v.Slice.step = step;
    return p;
}