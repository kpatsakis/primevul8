rb_reg_match_pre(VALUE match)
{
    VALUE str;
    struct re_registers *regs;

    if (NIL_P(match)) return Qnil;
    match_check(match);
    regs = RMATCH_REGS(match);
    if (BEG(0) == -1) return Qnil;
    str = rb_str_subseq(RMATCH(match)->str, 0, BEG(0));
    if (OBJ_TAINTED(match)) OBJ_TAINT(str);
    return str;
}