dict_real_result(i_ctx_t *i_ctx_p, ref * pdict, const char *kstr, double val)
{
    int code = 0;
    ref *ignore;

    if (dict_find_string(pdict, kstr, &ignore) > 0) {
        ref rval;

        check_dict_write(*pdict);
        make_real(&rval, val);
        code = idict_put_string(pdict, kstr, &rval);
    }
    return code;
}