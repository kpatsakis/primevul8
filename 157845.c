PyRun_SimpleString(const char *s)
{
    return PyRun_SimpleStringFlags(s, NULL);
}