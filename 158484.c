smtp_setup_batch_msg(void)
{
int done = 0;
void *reset_point = store_get(0);

/* Save the line count at the start of each transaction - single commands
like HELO and RSET count as whole transactions. */

bsmtp_transaction_linecount = receive_linecount;

if ((receive_feof)()) return 0;   /* Treat EOF as QUIT */

cancel_cutthrough_connection(TRUE, US"smtp_setup_batch_msg");
smtp_reset(reset_point);                /* Reset for start of message */

/* Deal with SMTP commands. This loop is exited by setting done to a POSITIVE
value. The values are 2 larger than the required yield of the function. */

while (done <= 0)
  {
  uschar *errmess;
  uschar *recipient = NULL;
  int start, end, sender_domain, recipient_domain;

  switch(smtp_read_command(FALSE, GETC_BUFFER_UNLIMITED))
    {
    /* The HELO/EHLO commands set sender_address_helo if they have
    valid data; otherwise they are ignored, except that they do
    a reset of the state. */

    case HELO_CMD:
    case EHLO_CMD:

      check_helo(smtp_cmd_data);
      /* Fall through */

    case RSET_CMD:
      cancel_cutthrough_connection(TRUE, US"RSET received");
      smtp_reset(reset_point);
      bsmtp_transaction_linecount = receive_linecount;
      break;


    /* The MAIL FROM command requires an address as an operand. All we
    do here is to parse it for syntactic correctness. The form "<>" is
    a special case which converts into an empty string. The start/end
    pointers in the original are not used further for this address, as
    it is the canonical extracted address which is all that is kept. */

    case MAIL_CMD:
      smtp_mailcmd_count++;              /* Count for no-mail log */
      if (sender_address != NULL)
	/* The function moan_smtp_batch() does not return. */
	moan_smtp_batch(smtp_cmd_buffer, "503 Sender already given");

      if (smtp_cmd_data[0] == 0)
	/* The function moan_smtp_batch() does not return. */
	moan_smtp_batch(smtp_cmd_buffer, "501 MAIL FROM must have an address operand");

      /* Reset to start of message */

      cancel_cutthrough_connection(TRUE, US"MAIL received");
      smtp_reset(reset_point);

      /* Apply SMTP rewrite */

      raw_sender = ((rewrite_existflags & rewrite_smtp) != 0)?
	rewrite_one(smtp_cmd_data, rewrite_smtp|rewrite_smtp_sender, NULL, FALSE,
	  US"", global_rewrite_rules) : smtp_cmd_data;

      /* Extract the address; the TRUE flag allows <> as valid */

      raw_sender =
	parse_extract_address(raw_sender, &errmess, &start, &end, &sender_domain,
	  TRUE);

      if (!raw_sender)
	/* The function moan_smtp_batch() does not return. */
	moan_smtp_batch(smtp_cmd_buffer, "501 %s", errmess);

      sender_address = string_copy(raw_sender);

      /* Qualify unqualified sender addresses if permitted to do so. */

      if (  !sender_domain
         && sender_address[0] != 0 && sender_address[0] != '@')
	if (f.allow_unqualified_sender)
	  {
	  sender_address = rewrite_address_qualify(sender_address, FALSE);
	  DEBUG(D_receive) debug_printf("unqualified address %s accepted "
	    "and rewritten\n", raw_sender);
	  }
	/* The function moan_smtp_batch() does not return. */
	else
	  moan_smtp_batch(smtp_cmd_buffer, "501 sender address must contain "
	    "a domain");
      break;


    /* The RCPT TO command requires an address as an operand. All we do
    here is to parse it for syntactic correctness. There may be any number
    of RCPT TO commands, specifying multiple senders. We build them all into
    a data structure that is in argc/argv format. The start/end values
    given by parse_extract_address are not used, as we keep only the
    extracted address. */

    case RCPT_CMD:
      if (!sender_address)
	/* The function moan_smtp_batch() does not return. */
	moan_smtp_batch(smtp_cmd_buffer, "503 No sender yet given");

      if (smtp_cmd_data[0] == 0)
	/* The function moan_smtp_batch() does not return. */
	moan_smtp_batch(smtp_cmd_buffer,
	  "501 RCPT TO must have an address operand");

      /* Check maximum number allowed */

      if (recipients_max > 0 && recipients_count + 1 > recipients_max)
	/* The function moan_smtp_batch() does not return. */
	moan_smtp_batch(smtp_cmd_buffer, "%s too many recipients",
	  recipients_max_reject? "552": "452");

      /* Apply SMTP rewrite, then extract address. Don't allow "<>" as a
      recipient address */

      recipient = rewrite_existflags & rewrite_smtp
	? rewrite_one(smtp_cmd_data, rewrite_smtp, NULL, FALSE, US"",
		      global_rewrite_rules)
	: smtp_cmd_data;

      recipient = parse_extract_address(recipient, &errmess, &start, &end,
	&recipient_domain, FALSE);

      if (!recipient)
	/* The function moan_smtp_batch() does not return. */
	moan_smtp_batch(smtp_cmd_buffer, "501 %s", errmess);

      /* If the recipient address is unqualified, qualify it if permitted. Then
      add it to the list of recipients. */

      if (!recipient_domain)
	if (f.allow_unqualified_recipient)
	  {
	  DEBUG(D_receive) debug_printf("unqualified address %s accepted\n",
	    recipient);
	  recipient = rewrite_address_qualify(recipient, TRUE);
	  }
	/* The function moan_smtp_batch() does not return. */
	else
	  moan_smtp_batch(smtp_cmd_buffer,
	    "501 recipient address must contain a domain");

      receive_add_recipient(recipient, -1);
      break;


    /* The DATA command is legal only if it follows successful MAIL FROM
    and RCPT TO commands. This function is complete when a valid DATA
    command is encountered. */

    case DATA_CMD:
      if (!sender_address || recipients_count <= 0)
	/* The function moan_smtp_batch() does not return. */
	if (!sender_address)
	  moan_smtp_batch(smtp_cmd_buffer,
	    "503 MAIL FROM:<sender> command must precede DATA");
	else
	  moan_smtp_batch(smtp_cmd_buffer,
	    "503 RCPT TO:<recipient> must precede DATA");
      else
	{
	done = 3;                      /* DATA successfully achieved */
	message_ended = END_NOTENDED;  /* Indicate in middle of message */
	}
      break;


    /* The VRFY, EXPN, HELP, ETRN, and NOOP commands are ignored. */

    case VRFY_CMD:
    case EXPN_CMD:
    case HELP_CMD:
    case NOOP_CMD:
    case ETRN_CMD:
      bsmtp_transaction_linecount = receive_linecount;
      break;


    case EOF_CMD:
    case QUIT_CMD:
      done = 2;
      break;


    case BADARG_CMD:
      /* The function moan_smtp_batch() does not return. */
      moan_smtp_batch(smtp_cmd_buffer, "501 Unexpected argument data");
      break;


    case BADCHAR_CMD:
      /* The function moan_smtp_batch() does not return. */
      moan_smtp_batch(smtp_cmd_buffer, "501 Unexpected NULL in SMTP command");
      break;


    default:
      /* The function moan_smtp_batch() does not return. */
      moan_smtp_batch(smtp_cmd_buffer, "500 Command unrecognized");
      break;
    }
  }

return done - 2;  /* Convert yield values */
}