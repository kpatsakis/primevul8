forbidden_name(struct compiling *c, identifier name, const node *n,
               int full_checks)
{
    assert(PyUnicode_Check(name));
    const char * const *p = FORBIDDEN;
    if (!full_checks) {
        /* In most cases, the parser will protect True, False, and None
           from being assign to. */
        p += 3;
    }
    for (; *p; p++) {
        if (_PyUnicode_EqualToASCIIString(name, *p)) {
            ast_error(c, n, "cannot assign to %U", name);
            return 1;
        }
    }
    return 0;
}