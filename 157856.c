PyCompileString(const char *str, const char *filename, int start)
{
    return Py_CompileStringFlags(str, filename, start, NULL);
}