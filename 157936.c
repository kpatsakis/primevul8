parser_expr(PyST_Object *self, PyObject *args, PyObject *kw)
{
    NOTE(ARGUNUSED(self))
    return (parser_do_parse(args, kw, "s:expr", PyST_EXPR));
}