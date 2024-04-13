match_offset(VALUE match, VALUE n)
{
    int i = match_backref_number(match, n);
    struct re_registers *regs = RMATCH_REGS(match);

    match_check(match);
    if (i < 0 || regs->num_regs <= i)
	rb_raise(rb_eIndexError, "index %d out of matches", i);

    if (BEG(i) < 0)
	return rb_assoc_new(Qnil, Qnil);

    update_char_offset(match);
    return rb_assoc_new(INT2FIX(RMATCH(match)->rmatch->char_offset[i].beg),
			INT2FIX(RMATCH(match)->rmatch->char_offset[i].end));
}