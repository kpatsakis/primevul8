decodeModifyOtherKeys(int c)
{
    char_u  *p = typebuf.tb_buf + typebuf.tb_off;
    int	    idx;
    int	    form = 0;
    int	    argidx = 0;
    int	    arg[2] = {0, 0};

    // Recognize:
    // form 0: {lead}{key};{modifier}u
    // form 1: {lead}27;{modifier};{key}~
    if ((c == CSI || (c == ESC && *p == '[')) && typebuf.tb_len >= 4)
    {
	idx = (*p == '[');
	if (p[idx] == '2' && p[idx + 1] == '7' && p[idx + 2] == ';')
	{
	    form = 1;
	    idx += 3;
	}
	while (idx < typebuf.tb_len && argidx < 2)
	{
	    if (p[idx] == ';')
		++argidx;
	    else if (VIM_ISDIGIT(p[idx]))
		arg[argidx] = arg[argidx] * 10 + (p[idx] - '0');
	    else
		break;
	    ++idx;
	}
	if (idx < typebuf.tb_len
		&& p[idx] == (form == 1 ? '~' : 'u')
		&& argidx == 1)
	{
	    // Match, consume the code.
	    typebuf.tb_off += idx + 1;
	    typebuf.tb_len -= idx + 1;

	    mod_mask = decode_modifiers(arg[!form]);
	    c = merge_modifyOtherKeys(arg[form]);
	}
    }

    return c;
}