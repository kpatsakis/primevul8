warn(const char *msg, const char *filename, int lineno)
{
    if (filename == NULL)
        filename = "<string>";
    PySys_WriteStderr(msg, filename, lineno);
}