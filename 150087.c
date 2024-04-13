match_values_at(int argc, VALUE *argv, VALUE match)
{
    VALUE result;
    int i;

    match_check(match);
    result = rb_ary_new2(argc);

    for (i=0; i<argc; i++) {
	if (FIXNUM_P(argv[i])) {
	    rb_ary_push(result, rb_reg_nth_match(FIX2INT(argv[i]), match));
	}
	else {
	    int num = namev_to_backref_number(RMATCH_REGS(match), RMATCH(match)->regexp, argv[i]);
	    if (num >= 0) {
		rb_ary_push(result, rb_reg_nth_match(num, match));
	    }
	    else {
		match_ary_aref(match, argv[i], result);
	    }
	}
    }
    return result;
}