smtp_refill(unsigned lim)
{
int rc, save_errno;
if (!smtp_out) return FALSE;
fflush(smtp_out);
if (smtp_receive_timeout > 0) ALARM(smtp_receive_timeout);

/* Limit amount read, so non-message data is not fed to DKIM.
Take care to not touch the safety NUL at the end of the buffer. */

rc = read(fileno(smtp_in), smtp_inbuffer, MIN(IN_BUFFER_SIZE-1, lim));
save_errno = errno;
if (smtp_receive_timeout > 0) ALARM_CLR(0);
if (rc <= 0)
  {
  /* Must put the error text in fixed store, because this might be during
  header reading, where it releases unused store above the header. */
  if (rc < 0)
    {
    if (had_command_timeout)		/* set by signal handler */
      smtp_command_timeout_exit();	/* does not return */
    if (had_command_sigterm)
      smtp_command_sigterm_exit();
    if (had_data_timeout)
      smtp_data_timeout_exit();
    if (had_data_sigint)
      smtp_data_sigint_exit();

    smtp_had_error = save_errno;
    smtp_read_error = string_copy_malloc(
      string_sprintf(" (error: %s)", strerror(save_errno)));
    }
  else
    smtp_had_eof = 1;
  return FALSE;
  }
#ifndef DISABLE_DKIM
dkim_exim_verify_feed(smtp_inbuffer, rc);
#endif
smtp_inend = smtp_inbuffer + rc;
smtp_inptr = smtp_inbuffer;
return TRUE;
}