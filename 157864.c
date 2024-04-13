parser_newstobject(node *st, int type)
{
    PyST_Object* o = PyObject_New(PyST_Object, &PyST_Type);

    if (o != 0) {
        o->st_node = st;
        o->st_type = type;
        o->st_flags.cf_flags = 0;
    }
    else {
        PyNode_Free(st);
    }
    return ((PyObject*)o);
}