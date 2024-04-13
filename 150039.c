name_to_backref_number(struct re_registers *regs, VALUE regexp, const char* name, const char* name_end)
{
    return onig_name_to_backref_number(RREGEXP_PTR(regexp),
	(const unsigned char* )name, (const unsigned char* )name_end, regs);
}