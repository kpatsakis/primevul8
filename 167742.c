static void set_basicauth_error(krb5_context context, krb5_error_code code)
{
    PyErr_SetObject(BasicAuthException_class, Py_BuildValue("(s:i)", krb5_get_err_text(context, code), code));
}