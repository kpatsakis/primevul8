err_string(const char *message)
{
    PyErr_SetString(parser_error, message);
}