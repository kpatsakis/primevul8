smtp_vprintf(const char *format, BOOL more, va_list ap)
{
gstring gs = { .size = big_buffer_size, .ptr = 0, .s = big_buffer };
BOOL yield;

yield = !! string_vformat(&gs, FALSE, format, ap);
string_from_gstring(&gs);

DEBUG(D_receive)
  {
  void *reset_point = store_get(0);
  uschar *msg_copy, *cr, *end;
  msg_copy = string_copy(gs.s);
  end = msg_copy + gs.ptr;
  while ((cr = Ustrchr(msg_copy, '\r')) != NULL)   /* lose CRs */
    memmove(cr, cr + 1, (end--) - cr);
  debug_printf("SMTP>> %s", msg_copy);
  store_reset(reset_point);
  }

if (!yield)
  {
  log_write(0, LOG_MAIN|LOG_PANIC, "string too large in smtp_printf()");
  smtp_closedown(US"Unexpected error");
  exim_exit(EXIT_FAILURE, NULL);
  }

/* If this is the first output for a (non-batch) RCPT command, see if all RCPTs
have had the same. Note: this code is also present in smtp_respond(). It would
be tidier to have it only in one place, but when it was added, it was easier to
do it that way, so as not to have to mess with the code for the RCPT command,
which sometimes uses smtp_printf() and sometimes smtp_respond(). */

if (fl.rcpt_in_progress)
  {
  if (rcpt_smtp_response == NULL)
    rcpt_smtp_response = string_copy(big_buffer);
  else if (fl.rcpt_smtp_response_same &&
           Ustrcmp(rcpt_smtp_response, big_buffer) != 0)
    fl.rcpt_smtp_response_same = FALSE;
  fl.rcpt_in_progress = FALSE;
  }

/* Now write the string */

#ifdef SUPPORT_TLS
if (tls_in.active.sock >= 0)
  {
  if (tls_write(NULL, gs.s, gs.ptr, more) < 0)
    smtp_write_error = -1;
  }
else
#endif

if (fprintf(smtp_out, "%s", gs.s) < 0) smtp_write_error = -1;
}