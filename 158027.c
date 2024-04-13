PyAST_Validate(mod_ty mod)
{
    int res = 0;

    switch (mod->kind) {
    case Module_kind:
        res = validate_stmts(mod->v.Module.body);
        break;
    case Interactive_kind:
        res = validate_stmts(mod->v.Interactive.body);
        break;
    case Expression_kind:
        res = validate_expr(mod->v.Expression.body, Load);
        break;
    case Suite_kind:
        PyErr_SetString(PyExc_ValueError, "Suite is not valid in the CPython compiler");
        break;
    default:
        PyErr_SetString(PyExc_SystemError, "impossible module node");
        res = 0;
        break;
    }
    return res;
}