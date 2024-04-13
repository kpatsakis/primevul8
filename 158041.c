PyObject* PyAST_mod2obj(mod_ty t)
{
    if (!init_types())
        return NULL;
    return ast2obj_mod(t);
}