rb_reg_prepare_re0(VALUE re, VALUE str, onig_errmsg_buffer err)
{
    regex_t *reg = RREGEXP_PTR(re);
    int r;
    OnigErrorInfo einfo;
    const char *pattern;
    VALUE unescaped;
    rb_encoding *fixed_enc = 0;
    rb_encoding *enc = rb_reg_prepare_enc(re, str, 1);

    if (reg->enc == enc) return reg;

    rb_reg_check(re);
    reg = RREGEXP_PTR(re);
    pattern = RREGEXP_SRC_PTR(re);

    unescaped = rb_reg_preprocess(
	pattern, pattern + RREGEXP_SRC_LEN(re), enc,
	&fixed_enc, err);

    if (unescaped == Qnil) {
	rb_raise(rb_eArgError, "regexp preprocess failed: %s", err);
    }

    r = onig_new(&reg, (UChar* )RSTRING_PTR(unescaped),
		 (UChar* )(RSTRING_PTR(unescaped) + RSTRING_LEN(unescaped)),
		 reg->options, enc,
		 OnigDefaultSyntax, &einfo);
    if (r) {
	onig_error_code_to_str((UChar*)err, r, &einfo);
	rb_reg_raise(pattern, RREGEXP_SRC_LEN(re), err, re);
    }

    RB_GC_GUARD(unescaped);
    return reg;
}