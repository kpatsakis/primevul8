parser_tuple2st(PyST_Object *self, PyObject *args, PyObject *kw)
{
    NOTE(ARGUNUSED(self))
    PyObject *st = 0;
    PyObject *tuple;
    node *tree;

    static char *keywords[] = {"sequence", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kw, "O:sequence2st", keywords,
                                     &tuple))
        return (0);
    if (!PySequence_Check(tuple)) {
        PyErr_SetString(PyExc_ValueError,
                        "sequence2st() requires a single sequence argument");
        return (0);
    }
    /*
     *  Convert the tree to the internal form before checking it.
     */
    tree = build_node_tree(tuple);
    if (tree != 0) {
        node *validation_root = NULL;
        int tree_type = 0;
        switch (TYPE(tree)) {
        case eval_input:
            /*  Might be an eval form.  */
            tree_type = PyST_EXPR;
            validation_root = tree;
            break;
        case encoding_decl:
            /* This looks like an encoding_decl so far. */
            if (NCH(tree) == 1) {
                tree_type = PyST_SUITE;
                validation_root = CHILD(tree, 0);
            }
            else {
                err_string("Error Parsing encoding_decl");
            }
            break;
        case file_input:
            /*  This looks like an exec form so far.  */
            tree_type = PyST_SUITE;
            validation_root = tree;
            break;
        default:
            /*  This is a fragment, at best. */
            err_string("parse tree does not use a valid start symbol");
        }

        if (validation_root != NULL && validate_node(validation_root))
            st = parser_newstobject(tree, tree_type);
        else
            PyNode_Free(tree);
    }
    /*  Make sure we raise an exception on all errors.  We should never
     *  get this, but we'd do well to be sure something is done.
     */
    if (st == NULL && !PyErr_Occurred())
        err_string("unspecified ST error occurred");

    return st;
}