match_equal(VALUE match1, VALUE match2)
{
    const struct re_registers *regs1, *regs2;
    if (match1 == match2) return Qtrue;
    if (!RB_TYPE_P(match2, T_MATCH)) return Qfalse;
    if (!RMATCH(match1)->regexp || !RMATCH(match2)->regexp) return Qfalse;
    if (!rb_str_equal(RMATCH(match1)->str, RMATCH(match2)->str)) return Qfalse;
    if (!rb_reg_equal(RMATCH(match1)->regexp, RMATCH(match2)->regexp)) return Qfalse;
    regs1 = RMATCH_REGS(match1);
    regs2 = RMATCH_REGS(match2);
    if (regs1->num_regs != regs2->num_regs) return Qfalse;
    if (memcmp(regs1->beg, regs2->beg, regs1->num_regs * sizeof(*regs1->beg))) return Qfalse;
    if (memcmp(regs1->end, regs2->end, regs1->num_regs * sizeof(*regs1->end))) return Qfalse;
    return Qtrue;
}