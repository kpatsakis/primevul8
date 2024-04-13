parser_free(PyST_Object *st)
{
    PyNode_Free(st->st_node);
    PyObject_Del(st);
}