reg_enc_error(VALUE re, VALUE str)
{
    rb_raise(rb_eEncCompatError,
	     "incompatible encoding regexp match (%s regexp with %s string)",
	     rb_enc_name(rb_enc_get(re)),
	     rb_enc_name(rb_enc_get(str)));
}