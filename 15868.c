do_tags(exarg_T *eap UNUSED)
{
    int		i;
    char_u	*name;
    taggy_T	*tagstack = curwin->w_tagstack;
    int		tagstackidx = curwin->w_tagstackidx;
    int		tagstacklen = curwin->w_tagstacklen;

    // Highlight title
    msg_puts_title(_("\n  # TO tag         FROM line  in file/text"));
    for (i = 0; i < tagstacklen; ++i)
    {
	if (tagstack[i].tagname != NULL)
	{
	    name = fm_getname(&(tagstack[i].fmark), 30);
	    if (name == NULL)	    // file name not available
		continue;

	    msg_putchar('\n');
	    vim_snprintf((char *)IObuff, IOSIZE, "%c%2d %2d %-15s %5ld  ",
		i == tagstackidx ? '>' : ' ',
		i + 1,
		tagstack[i].cur_match + 1,
		tagstack[i].tagname,
		tagstack[i].fmark.mark.lnum);
	    msg_outtrans(IObuff);
	    msg_outtrans_attr(name, tagstack[i].fmark.fnum == curbuf->b_fnum
							? HL_ATTR(HLF_D) : 0);
	    vim_free(name);
	}
	out_flush();		    // show one line at a time
    }
    if (tagstackidx == tagstacklen)	// idx at top of stack
	msg_puts("\n>");
}