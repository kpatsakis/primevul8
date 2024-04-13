rb_reg_regcomp(VALUE str)
{
    if (reg_cache && RREGEXP_SRC_LEN(reg_cache) == RSTRING_LEN(str)
	&& ENCODING_GET(reg_cache) == ENCODING_GET(str)
	&& memcmp(RREGEXP_SRC_PTR(reg_cache), RSTRING_PTR(str), RSTRING_LEN(str)) == 0)
	return reg_cache;

    return reg_cache = rb_reg_new_str(str, 0);
}