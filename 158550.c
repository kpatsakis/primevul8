bdat_getc(unsigned lim)
{
uschar * user_msg = NULL;
uschar * log_msg;

for(;;)
  {
#ifndef DISABLE_DKIM
  unsigned dkim_save;
#endif

  if (chunking_data_left > 0)
    return lwr_receive_getc(chunking_data_left--);

  receive_getc = lwr_receive_getc;
  receive_getbuf = lwr_receive_getbuf;
  receive_ungetc = lwr_receive_ungetc;
#ifndef DISABLE_DKIM
  dkim_save = dkim_collect_input;
  dkim_collect_input = 0;
#endif

  /* Unless PIPELINING was offered, there should be no next command
  until after we ack that chunk */

  if (!f.smtp_in_pipelining_advertised && !check_sync())
    {
    unsigned n = smtp_inend - smtp_inptr;
    if (n > 32) n = 32;

    incomplete_transaction_log(US"sync failure");
    log_write(0, LOG_MAIN|LOG_REJECT, "SMTP protocol synchronization error "
      "(next input sent too soon: pipelining was not advertised): "
      "rejected \"%s\" %s next input=\"%s\"%s",
      smtp_cmd_buffer, host_and_ident(TRUE),
      string_printing(string_copyn(smtp_inptr, n)),
      smtp_inend - smtp_inptr > n ? "..." : "");
    (void) synprot_error(L_smtp_protocol_error, 554, NULL,
      US"SMTP synchronization error");
    goto repeat_until_rset;
    }

  /* If not the last, ack the received chunk.  The last response is delayed
  until after the data ACL decides on it */

  if (chunking_state == CHUNKING_LAST)
    {
#ifndef DISABLE_DKIM
    dkim_exim_verify_feed(NULL, 0);	/* notify EOD */
#endif
    return EOD;
    }

  smtp_printf("250 %u byte chunk received\r\n", FALSE, chunking_datasize);
  chunking_state = CHUNKING_OFFERED;
  DEBUG(D_receive) debug_printf("chunking state %d\n", (int)chunking_state);

  /* Expect another BDAT cmd from input. RFC 3030 says nothing about
  QUIT, RSET or NOOP but handling them seems obvious */

next_cmd:
  switch(smtp_read_command(TRUE, 1))
    {
    default:
      (void) synprot_error(L_smtp_protocol_error, 503, NULL,
	US"only BDAT permissible after non-LAST BDAT");

  repeat_until_rset:
      switch(smtp_read_command(TRUE, 1))
	{
	case QUIT_CMD:	smtp_quit_handler(&user_msg, &log_msg);	/*FALLTHROUGH */
	case EOF_CMD:	return EOF;
	case RSET_CMD:	smtp_rset_handler(); return ERR;
	default:	if (synprot_error(L_smtp_protocol_error, 503, NULL,
					  US"only RSET accepted now") > 0)
			  return EOF;
			goto repeat_until_rset;
	}

    case QUIT_CMD:
      smtp_quit_handler(&user_msg, &log_msg);
      /*FALLTHROUGH*/
    case EOF_CMD:
      return EOF;

    case RSET_CMD:
      smtp_rset_handler();
      return ERR;

    case NOOP_CMD:
      HAD(SCH_NOOP);
      smtp_printf("250 OK\r\n", FALSE);
      goto next_cmd;

    case BDAT_CMD:
      {
      int n;

      if (sscanf(CS smtp_cmd_data, "%u %n", &chunking_datasize, &n) < 1)
	{
	(void) synprot_error(L_smtp_protocol_error, 501, NULL,
	  US"missing size for BDAT command");
	return ERR;
	}
      chunking_state = strcmpic(smtp_cmd_data+n, US"LAST") == 0
	? CHUNKING_LAST : CHUNKING_ACTIVE;
      chunking_data_left = chunking_datasize;
      DEBUG(D_receive) debug_printf("chunking state %d, %d bytes\n",
				    (int)chunking_state, chunking_data_left);

      if (chunking_datasize == 0)
	if (chunking_state == CHUNKING_LAST)
	  return EOD;
	else
	  {
	  (void) synprot_error(L_smtp_protocol_error, 504, NULL,
	    US"zero size for BDAT command");
	  goto repeat_until_rset;
	  }

      receive_getc = bdat_getc;
      receive_getbuf = bdat_getbuf;	/* r~getbuf is never actually used */
      receive_ungetc = bdat_ungetc;
#ifndef DISABLE_DKIM
      dkim_collect_input = dkim_save;
#endif
      break;	/* to top of main loop */
      }
    }
  }
}