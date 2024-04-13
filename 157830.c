Py_CompileString(const char *str, const char *p, int s)
{
    return Py_CompileStringExFlags(str, p, s, NULL, -1);
}