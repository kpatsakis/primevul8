ast_for_classdef(struct compiling *c, const node *n, asdl_seq *decorator_seq)
{
    /* classdef: 'class' NAME ['(' arglist ')'] ':' suite */
    PyObject *classname;
    asdl_seq *s;
    expr_ty call;
    int end_lineno, end_col_offset;

    REQ(n, classdef);

    if (NCH(n) == 4) { /* class NAME ':' suite */
        s = ast_for_suite(c, CHILD(n, 3));
        if (!s)
            return NULL;
        get_last_end_pos(s, &end_lineno, &end_col_offset);

        classname = NEW_IDENTIFIER(CHILD(n, 1));
        if (!classname)
            return NULL;
        if (forbidden_name(c, classname, CHILD(n, 3), 0))
            return NULL;
        return ClassDef(classname, NULL, NULL, s, decorator_seq,
                        LINENO(n), n->n_col_offset,
                        end_lineno, end_col_offset, c->c_arena);
    }

    if (TYPE(CHILD(n, 3)) == RPAR) { /* class NAME '(' ')' ':' suite */
        s = ast_for_suite(c, CHILD(n, 5));
        if (!s)
            return NULL;
        get_last_end_pos(s, &end_lineno, &end_col_offset);

        classname = NEW_IDENTIFIER(CHILD(n, 1));
        if (!classname)
            return NULL;
        if (forbidden_name(c, classname, CHILD(n, 3), 0))
            return NULL;
        return ClassDef(classname, NULL, NULL, s, decorator_seq,
                        LINENO(n), n->n_col_offset,
                        end_lineno, end_col_offset, c->c_arena);
    }

    /* class NAME '(' arglist ')' ':' suite */
    /* build up a fake Call node so we can extract its pieces */
    {
        PyObject *dummy_name;
        expr_ty dummy;
        dummy_name = NEW_IDENTIFIER(CHILD(n, 1));
        if (!dummy_name)
            return NULL;
        dummy = Name(dummy_name, Load, LINENO(n), n->n_col_offset,
                     CHILD(n, 1)->n_end_lineno, CHILD(n, 1)->n_end_col_offset,
                     c->c_arena);
        call = ast_for_call(c, CHILD(n, 3), dummy, NULL, CHILD(n, 4));
        if (!call)
            return NULL;
    }
    s = ast_for_suite(c, CHILD(n, 6));
    if (!s)
        return NULL;
    get_last_end_pos(s, &end_lineno, &end_col_offset);

    classname = NEW_IDENTIFIER(CHILD(n, 1));
    if (!classname)
        return NULL;
    if (forbidden_name(c, classname, CHILD(n, 1), 0))
        return NULL;

    return ClassDef(classname, call->v.Call.args, call->v.Call.keywords, s,
                    decorator_seq, LINENO(n), n->n_col_offset,
                    end_lineno, end_col_offset, c->c_arena);
}