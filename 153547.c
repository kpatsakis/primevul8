ex_efct(int ex)
{
    int effect = 0;

    if (! ex)
	return 0;

    if (ex & PE_EX_ITALIC)
	effect |= PE_EX_ITALIC_E;

    if (ex & PE_EX_INSERT)
	effect |= PE_EX_INSERT_E;

    if (ex & PE_EX_STRIKE)
	effect |= PE_EX_STRIKE_E;

    return effect;
}