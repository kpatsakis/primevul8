PyRun_String(const char *str, int s, PyObject *g, PyObject *l)
{
    return PyRun_StringFlags(str, s, g, l, NULL);
}