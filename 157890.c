Py_CompileStringFlags(const char *str, const char *p, int s,
                      PyCompilerFlags *flags)
{
    return Py_CompileStringExFlags(str, p, s, flags, -1);
}