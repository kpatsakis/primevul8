ast_for_factor(struct compiling *c, const node *n)
{
    node *pfactor, *ppower, *patom, *pnum;
    expr_ty expression;

    /* If the unary - operator is applied to a constant, don't generate
       a UNARY_NEGATIVE opcode.  Just store the approriate value as a
       constant.  The peephole optimizer already does something like
       this but it doesn't handle the case where the constant is
       (sys.maxint - 1).  In that case, we want a PyIntObject, not a
       PyLongObject.
    */
    if (TYPE(CHILD(n, 0)) == MINUS &&
        NCH(n) == 2 &&
        TYPE((pfactor = CHILD(n, 1))) == factor &&
        NCH(pfactor) == 1 &&
        TYPE((ppower = CHILD(pfactor, 0))) == power &&
        NCH(ppower) == 1 &&
        TYPE((patom = CHILD(ppower, 0))) == atom &&
        TYPE((pnum = CHILD(patom, 0))) == NUMBER) {
        PyObject *pynum;
        char *s = PyObject_MALLOC(strlen(STR(pnum)) + 2);
        if (s == NULL)
            return NULL;
        s[0] = '-';
        strcpy(s + 1, STR(pnum));
        pynum = parsenumber(c, s);
        PyObject_FREE(s);
        if (!pynum)
            return NULL;

        PyArena_AddPyObject(c->c_arena, pynum);
        return Num(pynum, LINENO(n), n->n_col_offset, c->c_arena);
    }

    expression = ast_for_expr(c, CHILD(n, 1));
    if (!expression)
        return NULL;

    switch (TYPE(CHILD(n, 0))) {
        case PLUS:
            return UnaryOp(UAdd, expression, LINENO(n), n->n_col_offset,
                           c->c_arena);
        case MINUS:
            return UnaryOp(USub, expression, LINENO(n), n->n_col_offset,
                           c->c_arena);
        case TILDE:
            return UnaryOp(Invert, expression, LINENO(n),
                           n->n_col_offset, c->c_arena);
    }
    PyErr_Format(PyExc_SystemError, "unhandled factor: %d",
                 TYPE(CHILD(n, 0)));
    return NULL;
}