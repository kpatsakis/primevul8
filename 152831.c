arguments(asdl_seq * args, arg_ty vararg, asdl_seq * kwonlyargs, asdl_seq *
          kw_defaults, arg_ty kwarg, asdl_seq * defaults, PyArena *arena)
{
    arguments_ty p;
    p = (arguments_ty)PyArena_Malloc(arena, sizeof(*p));
    if (!p)
        return NULL;
    p->args = args;
    p->vararg = vararg;
    p->kwonlyargs = kwonlyargs;
    p->kw_defaults = kw_defaults;
    p->kwarg = kwarg;
    p->defaults = defaults;
    return p;
}