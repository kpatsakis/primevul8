validate_keywords(asdl_seq *keywords)
{
    Py_ssize_t i;
    for (i = 0; i < asdl_seq_LEN(keywords); i++)
        if (!validate_expr(((keyword_ty)asdl_seq_GET(keywords, i))->value, Load))
            return 0;
    return 1;
}