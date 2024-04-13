err_free(perrdetail *err)
{
    Py_CLEAR(err->filename);
}