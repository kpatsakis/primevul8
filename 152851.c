ast_traverse(AST_object *self, visitproc visit, void *arg)
{
    Py_VISIT(self->dict);
    return 0;
}