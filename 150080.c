rb_reg_error_desc(VALUE str, int options, const char *err)
{
    return rb_enc_reg_error_desc(RSTRING_PTR(str), RSTRING_LEN(str),
				 rb_enc_get(str), options, err);
}