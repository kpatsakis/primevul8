validate_args(asdl_seq *args)
{
    Py_ssize_t i;
    for (i = 0; i < asdl_seq_LEN(args); i++) {
        arg_ty arg = asdl_seq_GET(args, i);
        if (arg->annotation && !validate_expr(arg->annotation, Load))
            return 0;
    }
    return 1;
}