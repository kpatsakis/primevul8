ast_clear(AST_object *self)
{
    Py_CLEAR(self->dict);
    return 0;
}