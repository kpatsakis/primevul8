mb_replace_pop_ins(int cc)
{
    int		n;
    char_u	buf[MB_MAXBYTES + 1];
    int		i;
    int		c;

    if (has_mbyte && (n = MB_BYTE2LEN(cc)) > 1)
    {
	buf[0] = cc;
	for (i = 1; i < n; ++i)
	    buf[i] = replace_pop();
	ins_bytes_len(buf, n);
    }
    else
	ins_char(cc);

    if (enc_utf8)
	// Handle composing chars.
	for (;;)
	{
	    c = replace_pop();
	    if (c == -1)	    // stack empty
		break;
	    if ((n = MB_BYTE2LEN(c)) == 1)
	    {
		// Not a multi-byte char, put it back.
		replace_push(c);
		break;
	    }
	    else
	    {
		buf[0] = c;
		for (i = 1; i < n; ++i)
		    buf[i] = replace_pop();
		if (utf_iscomposing(utf_ptr2char(buf)))
		    ins_bytes_len(buf, n);
		else
		{
		    // Not a composing char, put it back.
		    for (i = n - 1; i >= 0; --i)
			replace_push(buf[i]);
		    break;
		}
	    }
	}
}