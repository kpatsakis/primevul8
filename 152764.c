ast_dealloc(AST_object *self)
{
    /* bpo-31095: UnTrack is needed before calling any callbacks */
    PyObject_GC_UnTrack(self);
    Py_CLEAR(self->dict);
    Py_TYPE(self)->tp_free(self);
}