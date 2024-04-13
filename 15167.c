ex_execute(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    typval_T	rettv;
    int		ret = OK;
    char_u	*p;
    garray_T	ga;
    int		len;
    long	start_lnum = SOURCING_LNUM;

    ga_init2(&ga, 1, 80);

    if (eap->skip)
	++emsg_skip;
    while (!ends_excmd2(eap->cmd, arg) || *arg == '"')
    {
	ret = eval1_emsg(&arg, &rettv, eap);
	if (ret == FAIL)
	    break;

	if (!eap->skip)
	{
	    char_u   buf[NUMBUFLEN];

	    if (eap->cmdidx == CMD_execute)
	    {
		if (rettv.v_type == VAR_CHANNEL || rettv.v_type == VAR_JOB)
		{
		    semsg(_(e_using_invalid_value_as_string_str),
						  vartype_name(rettv.v_type));
		    p = NULL;
		}
		else
		    p = tv_get_string_buf(&rettv, buf);
	    }
	    else
		p = tv_stringify(&rettv, buf);
	    if (p == NULL)
	    {
		clear_tv(&rettv);
		ret = FAIL;
		break;
	    }
	    len = (int)STRLEN(p);
	    if (ga_grow(&ga, len + 2) == FAIL)
	    {
		clear_tv(&rettv);
		ret = FAIL;
		break;
	    }
	    if (ga.ga_len)
		((char_u *)(ga.ga_data))[ga.ga_len++] = ' ';
	    STRCPY((char_u *)(ga.ga_data) + ga.ga_len, p);
	    ga.ga_len += len;
	}

	clear_tv(&rettv);
	arg = skipwhite(arg);
    }

    if (ret != FAIL && ga.ga_data != NULL)
    {
	// use the first line of continuation lines for messages
	SOURCING_LNUM = start_lnum;

	if (eap->cmdidx == CMD_echomsg || eap->cmdidx == CMD_echoerr)
	{
	    // Mark the already saved text as finishing the line, so that what
	    // follows is displayed on a new line when scrolling back at the
	    // more prompt.
	    msg_sb_eol();
	}

	if (eap->cmdidx == CMD_echomsg)
	{
	    msg_attr(ga.ga_data, echo_attr);
	    out_flush();
	}
	else if (eap->cmdidx == CMD_echoconsole)
	{
	    ui_write(ga.ga_data, (int)STRLEN(ga.ga_data), TRUE);
	    ui_write((char_u *)"\r\n", 2, TRUE);
	}
	else if (eap->cmdidx == CMD_echoerr)
	{
	    int		save_did_emsg = did_emsg;

	    // We don't want to abort following commands, restore did_emsg.
	    emsg(ga.ga_data);
	    if (!force_abort)
		did_emsg = save_did_emsg;
	}
	else if (eap->cmdidx == CMD_execute)
	    do_cmdline((char_u *)ga.ga_data,
		       eap->getline, eap->cookie, DOCMD_NOWAIT|DOCMD_VERBOSE);
    }

    ga_clear(&ga);

    if (eap->skip)
	--emsg_skip;

    set_nextcmd(eap, arg);
}