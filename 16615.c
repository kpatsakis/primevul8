check_limit(VALUE str, VALUE opt)
{
    if (NIL_P(str)) return;
    if (SYMBOL_P(str)) str = rb_sym2str(str);

    StringValue(str);
    size_t slen = RSTRING_LEN(str);
    size_t limit = get_limit(opt);
    if (slen > limit) {
	rb_raise(rb_eArgError,
		 "string length (%"PRI_SIZE_PREFIX"u) exceeds the limit %"PRI_SIZE_PREFIX"u", slen, limit);
    }
}