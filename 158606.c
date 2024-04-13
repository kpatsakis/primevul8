lmtp_transport_entry(
  transport_instance *tblock,      /* data for this instantiation */
  address_item *addrlist)          /* address(es) we are working on */
{
pid_t pid = 0;
FILE *out;
lmtp_transport_options_block *ob =
  (lmtp_transport_options_block *)(tblock->options_block);
struct sockaddr_un sockun;         /* don't call this "sun" ! */
int timeout = ob->timeout;
int fd_in = -1, fd_out = -1;
int code, save_errno;
BOOL send_data;
BOOL yield = FALSE;
address_item *addr;
uschar *igquotstr = US"";
uschar *sockname = NULL;
const uschar **argv;
uschar buffer[256];

DEBUG(D_transport) debug_printf("%s transport entered\n", tblock->name);

/* Initialization ensures that either a command or a socket is specified, but
not both. When a command is specified, call the common function for creating an
argument list and expanding the items. */

if (ob->cmd)
  {
  DEBUG(D_transport) debug_printf("using command %s\n", ob->cmd);
  sprintf(CS buffer, "%.50s transport", tblock->name);
  if (!transport_set_up_command(&argv, ob->cmd, TRUE, PANIC, addrlist, buffer,
       NULL))
    return FALSE;

  /* If the -N option is set, can't do any more. Presume all has gone well. */
  if (f.dont_deliver)
    goto MINUS_N;

/* As this is a local transport, we are already running with the required
uid/gid and current directory. Request that the new process be a process group
leader, so we can kill it and all its children on an error. */

  if ((pid = child_open(USS argv, NULL, 0, &fd_in, &fd_out, TRUE)) < 0)
    {
    addrlist->message = string_sprintf(
      "Failed to create child process for %s transport: %s", tblock->name,
        strerror(errno));
    return FALSE;
    }
  }

/* When a socket is specified, expand the string and create a socket. */

else
  {
  DEBUG(D_transport) debug_printf("using socket %s\n", ob->skt);
  sockname = expand_string(ob->skt);
  if (sockname == NULL)
    {
    addrlist->message = string_sprintf("Expansion of \"%s\" (socket setting "
      "for %s transport) failed: %s", ob->skt, tblock->name,
      expand_string_message);
    return FALSE;
    }
  if ((fd_in = fd_out = socket(PF_UNIX, SOCK_STREAM, 0)) == -1)
    {
    addrlist->message = string_sprintf(
      "Failed to create socket %s for %s transport: %s",
        ob->skt, tblock->name, strerror(errno));
    return FALSE;
    }

  /* If the -N option is set, can't do any more. Presume all has gone well. */
  if (f.dont_deliver)
    goto MINUS_N;

  sockun.sun_family = AF_UNIX;
  sprintf(sockun.sun_path, "%.*s", (int)(sizeof(sockun.sun_path)-1), sockname);
  if(connect(fd_out, (struct sockaddr *)(&sockun), sizeof(sockun)) == -1)
    {
    addrlist->message = string_sprintf(
      "Failed to connect to socket %s for %s transport: %s",
        sockun.sun_path, tblock->name, strerror(errno));
    return FALSE;
    }
  }


/* Make the output we are going to read into a file. */

out = fdopen(fd_out, "rb");

/* Now we must implement the LMTP protocol. It is like SMTP, except that after
the end of the message, a return code for every accepted RCPT TO is sent. This
allows for message+recipient checks after the message has been received. */

/* First thing is to wait for an initial greeting. */

Ustrcpy(big_buffer, "initial connection");
if (!lmtp_read_response(out, buffer, sizeof(buffer), '2',
  timeout)) goto RESPONSE_FAILED;

/* Next, we send a LHLO command, and expect a positive response */

if (!lmtp_write_command(fd_in, "%s %s\r\n", "LHLO",
  primary_hostname)) goto WRITE_FAILED;

if (!lmtp_read_response(out, buffer, sizeof(buffer), '2',
     timeout)) goto RESPONSE_FAILED;

/* If the ignore_quota option is set, note whether the server supports the
IGNOREQUOTA option, and if so, set an appropriate addition for RCPT. */

if (ob->ignore_quota)
  igquotstr = (pcre_exec(regex_IGNOREQUOTA, NULL, CS buffer,
    Ustrlen(CS buffer), 0, PCRE_EOPT, NULL, 0) >= 0)? US" IGNOREQUOTA" : US"";

/* Now the envelope sender */

if (!lmtp_write_command(fd_in, "MAIL FROM:<%s>\r\n", return_path))
  goto WRITE_FAILED;

if (!lmtp_read_response(out, buffer, sizeof(buffer), '2', timeout))
  {
  if (errno == 0 && buffer[0] == '4')
    {
    errno = ERRNO_MAIL4XX;
    addrlist->more_errno |= ((buffer[1] - '0')*10 + buffer[2] - '0') << 8;
    }
  goto RESPONSE_FAILED;
  }

/* Next, we hand over all the recipients. Some may be permanently or
temporarily rejected; others may be accepted, for now. */

send_data = FALSE;
for (addr = addrlist; addr != NULL; addr = addr->next)
  {
  if (!lmtp_write_command(fd_in, "RCPT TO:<%s>%s\r\n",
       transport_rcpt_address(addr, tblock->rcpt_include_affixes), igquotstr))
    goto WRITE_FAILED;
  if (lmtp_read_response(out, buffer, sizeof(buffer), '2', timeout))
    {
    send_data = TRUE;
    addr->transport_return = PENDING_OK;
    }
  else
    {
    if (errno != 0 || buffer[0] == 0) goto RESPONSE_FAILED;
    addr->message = string_sprintf("LMTP error after %s: %s", big_buffer,
      string_printing(buffer));
    setflag(addr, af_pass_message);   /* Allow message to go to user */
    if (buffer[0] == '5') addr->transport_return = FAIL; else
      {
      addr->basic_errno = ERRNO_RCPT4XX;
      addr->more_errno |= ((buffer[1] - '0')*10 + buffer[2] - '0') << 8;
      }
    }
  }

/* Now send the text of the message if there were any good recipients. */

if (send_data)
  {
  BOOL ok;
  transport_ctx tctx = {
    {fd_in},
    tblock,
    addrlist,
    US".", US"..",
    ob->options
  };

  if (!lmtp_write_command(fd_in, "DATA\r\n")) goto WRITE_FAILED;
  if (!lmtp_read_response(out, buffer, sizeof(buffer), '3', timeout))
    {
    if (errno == 0 && buffer[0] == '4')
      {
      errno = ERRNO_DATA4XX;
      addrlist->more_errno |= ((buffer[1] - '0')*10 + buffer[2] - '0') << 8;
      }
    goto RESPONSE_FAILED;
    }

  sigalrm_seen = FALSE;
  transport_write_timeout = timeout;
  Ustrcpy(big_buffer, "sending data block");   /* For error messages */
  DEBUG(D_transport|D_v)
    debug_printf("  LMTP>> writing message and terminating \".\"\n");

  transport_count = 0;
  ok = transport_write_message(&tctx, 0);

  /* Failure can either be some kind of I/O disaster (including timeout),
  or the failure of a transport filter or the expansion of added headers. */

  if (!ok)
    {
    buffer[0] = 0;              /* There hasn't been a response */
    goto RESPONSE_FAILED;
    }

  Ustrcpy(big_buffer, "end of data");   /* For error messages */

  /* We now expect a response for every address that was accepted above,
  in the same order. For those that get a response, their status is fixed;
  any that are accepted have been handed over, even if later responses crash -
  at least, that's how I read RFC 2033. */

  for (addr = addrlist; addr != NULL; addr = addr->next)
    {
    if (addr->transport_return != PENDING_OK) continue;

    if (lmtp_read_response(out, buffer, sizeof(buffer), '2', timeout))
      {
      addr->transport_return = OK;
      if (LOGGING(smtp_confirmation))
        {
        const uschar *s = string_printing(buffer);
	/* de-const safe here as string_printing known to have alloc'n'copied */
        addr->message = (s == buffer)? US string_copy(s) : US s;
        }
      }
    /* If the response has failed badly, use it for all the remaining pending
    addresses and give up. */

    else if (errno != 0 || buffer[0] == 0)
      {
      address_item *a;
      save_errno = errno;
      check_response(&save_errno, addr->more_errno, buffer, &code,
        &(addr->message));
      addr->transport_return = (code == '5')? FAIL : DEFER;
      for (a = addr->next; a != NULL; a = a->next)
        {
        if (a->transport_return != PENDING_OK) continue;
        a->basic_errno = addr->basic_errno;
        a->message = addr->message;
        a->transport_return = addr->transport_return;
        }
      break;
      }

    /* Otherwise, it's an LMTP error code return for one address */

    else
      {
      if (buffer[0] == '4')
        {
        addr->basic_errno = ERRNO_DATA4XX;
        addr->more_errno |= ((buffer[1] - '0')*10 + buffer[2] - '0') << 8;
        }
      addr->message = string_sprintf("LMTP error after %s: %s", big_buffer,
        string_printing(buffer));
      addr->transport_return = (buffer[0] == '5')? FAIL : DEFER;
      setflag(addr, af_pass_message);   /* Allow message to go to user */
      }
    }
  }

/* The message transaction has completed successfully - this doesn't mean that
all the addresses have necessarily been transferred, but each has its status
set, so we change the yield to TRUE. */

yield = TRUE;
(void) lmtp_write_command(fd_in, "QUIT\r\n");
(void) lmtp_read_response(out, buffer, sizeof(buffer), '2', 1);

goto RETURN;


/* Come here if any call to read_response, other than a response after the data
phase, failed. Put the error in the top address - this will be replicated
because the yield is still FALSE. (But omit ETIMEDOUT, as there will already be
a suitable message.) Analyse the error, and if if isn't too bad, send a QUIT
command. Wait for the response with a short timeout, so we don't wind up this
process before the far end has had time to read the QUIT. */

RESPONSE_FAILED:

save_errno = errno;
if (errno != ETIMEDOUT && errno != 0) addrlist->basic_errno = errno;
addrlist->message = NULL;

if (check_response(&save_errno, addrlist->more_errno,
    buffer, &code, &(addrlist->message)))
  {
  (void) lmtp_write_command(fd_in, "QUIT\r\n");
  (void) lmtp_read_response(out, buffer, sizeof(buffer), '2', 1);
  }

addrlist->transport_return = (code == '5')? FAIL : DEFER;
if (code == '4' && save_errno > 0)
  addrlist->message = string_sprintf("%s: %s", addrlist->message,
    strerror(save_errno));
goto KILL_AND_RETURN;

/* Come here if there are errors during writing of a command or the message
itself. This error will be applied to all the addresses. */

WRITE_FAILED:

addrlist->transport_return = PANIC;
addrlist->basic_errno = errno;
if (errno == ERRNO_CHHEADER_FAIL)
  addrlist->message =
    string_sprintf("Failed to expand headers_add or headers_remove: %s",
      expand_string_message);
else if (errno == ERRNO_FILTER_FAIL)
  addrlist->message = string_sprintf("Filter process failure");
else if (errno == ERRNO_WRITEINCOMPLETE)
  addrlist->message = string_sprintf("Failed repeatedly to write data");
else if (errno == ERRNO_SMTPFORMAT)
  addrlist->message = US"overlong LMTP command generated";
else
  addrlist->message = string_sprintf("Error %d", errno);

/* Come here after errors. Kill off the process. */

KILL_AND_RETURN:

if (pid > 0) killpg(pid, SIGKILL);

/* Come here from all paths after the subprocess is created. Wait for the
process, but with a timeout. */

RETURN:

(void)child_close(pid, timeout);

if (fd_in >= 0) (void)close(fd_in);
if (fd_out >= 0) (void)fclose(out);

DEBUG(D_transport)
  debug_printf("%s transport yields %d\n", tblock->name, yield);

return yield;


MINUS_N:
  DEBUG(D_transport)
    debug_printf("*** delivery by %s transport bypassed by -N option",
      tblock->name);
  addrlist->transport_return = OK;
  return FALSE;
}