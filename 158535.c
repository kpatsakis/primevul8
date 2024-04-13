smtp_read_command(BOOL check_sync, unsigned buffer_lim)
{
int c;
int ptr = 0;
smtp_cmd_list *p;
BOOL hadnull = FALSE;

had_command_timeout = 0;
os_non_restarting_signal(SIGALRM, command_timeout_handler);

while ((c = (receive_getc)(buffer_lim)) != '\n' && c != EOF)
  {
  if (ptr >= SMTP_CMD_BUFFER_SIZE)
    {
    os_non_restarting_signal(SIGALRM, sigalrm_handler);
    return OTHER_CMD;
    }
  if (c == 0)
    {
    hadnull = TRUE;
    c = '?';
    }
  smtp_cmd_buffer[ptr++] = c;
  }

receive_linecount++;    /* For BSMTP errors */
os_non_restarting_signal(SIGALRM, sigalrm_handler);

/* If hit end of file, return pseudo EOF command. Whether we have a
part-line already read doesn't matter, since this is an error state. */

if (c == EOF) return EOF_CMD;

/* Remove any CR and white space at the end of the line, and terminate the
string. */

while (ptr > 0 && isspace(smtp_cmd_buffer[ptr-1])) ptr--;
smtp_cmd_buffer[ptr] = 0;

DEBUG(D_receive) debug_printf("SMTP<< %s\n", smtp_cmd_buffer);

/* NULLs are not allowed in SMTP commands */

if (hadnull) return BADCHAR_CMD;

/* Scan command list and return identity, having set the data pointer
to the start of the actual data characters. Check for SMTP synchronization
if required. */

for (p = cmd_list; p < cmd_list_end; p++)
  {
#ifdef SUPPORT_PROXY
  /* Only allow QUIT command if Proxy Protocol parsing failed */
  if (proxy_session && f.proxy_session_failed && p->cmd != QUIT_CMD)
    continue;
#endif
  if (  p->len
     && strncmpic(smtp_cmd_buffer, US p->name, p->len) == 0
     && (  smtp_cmd_buffer[p->len-1] == ':'    /* "mail from:" or "rcpt to:" */
        || smtp_cmd_buffer[p->len] == 0
	|| smtp_cmd_buffer[p->len] == ' '
     )  )
    {
    if (smtp_inptr < smtp_inend &&                     /* Outstanding input */
        p->cmd < sync_cmd_limit &&                     /* Command should sync */
        check_sync &&                                  /* Local flag set */
        smtp_enforce_sync &&                           /* Global flag set */
        sender_host_address != NULL &&                 /* Not local input */
        !f.sender_host_notsocket)                        /* Really is a socket */
      return BADSYN_CMD;

    /* The variables $smtp_command and $smtp_command_argument point into the
    unmodified input buffer. A copy of the latter is taken for actual
    processing, so that it can be chopped up into separate parts if necessary,
    for example, when processing a MAIL command options such as SIZE that can
    follow the sender address. */

    smtp_cmd_argument = smtp_cmd_buffer + p->len;
    while (isspace(*smtp_cmd_argument)) smtp_cmd_argument++;
    Ustrcpy(smtp_data_buffer, smtp_cmd_argument);
    smtp_cmd_data = smtp_data_buffer;

    /* Count non-mail commands from those hosts that are controlled in this
    way. The default is all hosts. We don't waste effort checking the list
    until we get a non-mail command, but then cache the result to save checking
    again. If there's a DEFER while checking the host, assume it's in the list.

    Note that one instance of RSET, EHLO/HELO, and STARTTLS is allowed at the
    start of each incoming message by fiddling with the value in the table. */

    if (!p->is_mail_cmd)
      {
      if (count_nonmail == TRUE_UNSET) count_nonmail =
        verify_check_host(&smtp_accept_max_nonmail_hosts) != FAIL;
      if (count_nonmail && ++nonmail_command_count > smtp_accept_max_nonmail)
        return TOO_MANY_NONMAIL_CMD;
      }

    /* If there is data for a command that does not expect it, generate the
    error here. */

    return (p->has_arg || *smtp_cmd_data == 0)? p->cmd : BADARG_CMD;
    }
  }

#ifdef SUPPORT_PROXY
/* Only allow QUIT command if Proxy Protocol parsing failed */
if (proxy_session && f.proxy_session_failed)
  return PROXY_FAIL_IGNORE_CMD;
#endif

/* Enforce synchronization for unknown commands */

if (  smtp_inptr < smtp_inend		/* Outstanding input */
   && check_sync			/* Local flag set */
   && smtp_enforce_sync			/* Global flag set */
   && sender_host_address		/* Not local input */
   && !f.sender_host_notsocket)		/* Really is a socket */
  return BADSYN_CMD;

return OTHER_CMD;
}