ast_for_classdef(struct compiling *c, const node *n, asdl_seq *decorator_seq)
{
    /* classdef: 'class' NAME ['(' testlist ')'] ':' suite */
    PyObject *classname;
    asdl_seq *bases, *s;

    REQ(n, classdef);

    if (!forbidden_check(c, n, STR(CHILD(n, 1))))
            return NULL;

    if (NCH(n) == 4) {
        s = ast_for_suite(c, CHILD(n, 3));
        if (!s)
            return NULL;
        classname = NEW_IDENTIFIER(CHILD(n, 1));
        if (!classname)
            return NULL;
        return ClassDef(classname, NULL, s, decorator_seq, LINENO(n),
                        n->n_col_offset, c->c_arena);
    }
    /* check for empty base list */
    if (TYPE(CHILD(n,3)) == RPAR) {
        s = ast_for_suite(c, CHILD(n,5));
        if (!s)
            return NULL;
        classname = NEW_IDENTIFIER(CHILD(n, 1));
        if (!classname)
            return NULL;
        return ClassDef(classname, NULL, s, decorator_seq, LINENO(n),
                        n->n_col_offset, c->c_arena);
    }

    /* else handle the base class list */
    bases = ast_for_class_bases(c, CHILD(n, 3));
    if (!bases)
        return NULL;

    s = ast_for_suite(c, CHILD(n, 6));
    if (!s)
        return NULL;
    classname = NEW_IDENTIFIER(CHILD(n, 1));
    if (!classname)
        return NULL;
    return ClassDef(classname, bases, s, decorator_seq,
                    LINENO(n), n->n_col_offset, c->c_arena);
}