forbidden_check(struct compiling *c, const node *n, const char *x)
{
    if (!strcmp(x, "None"))
        return ast_error(n, "cannot assign to None");
    if (!strcmp(x, "__debug__"))
        return ast_error(n, "cannot assign to __debug__");
    if (Py_Py3kWarningFlag) {
        if (!(strcmp(x, "True") && strcmp(x, "False")) &&
            !ast_warn(c, n, "assignment to True or False is forbidden in 3.x"))
            return 0;
        if (!strcmp(x, "nonlocal") &&
            !ast_warn(c, n, "nonlocal is a keyword in 3.x"))
            return 0;
    }
    return 1;
}