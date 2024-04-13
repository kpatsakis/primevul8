smtp_notquit_exit(uschar *reason, uschar *code, uschar *defaultrespond, ...)
{
int rc;
uschar *user_msg = NULL;
uschar *log_msg = NULL;

/* Check for recursive acll */

if (fl.smtp_exit_function_called)
  {
  log_write(0, LOG_PANIC, "smtp_notquit_exit() called more than once (%s)",
    reason);
  return;
  }
fl.smtp_exit_function_called = TRUE;

/* Call the not-QUIT ACL, if there is one, unless no reason is given. */

if (acl_smtp_notquit && reason)
  {
  smtp_notquit_reason = reason;
  if ((rc = acl_check(ACL_WHERE_NOTQUIT, NULL, acl_smtp_notquit, &user_msg,
		      &log_msg)) == ERROR)
    log_write(0, LOG_MAIN|LOG_PANIC, "ACL for not-QUIT returned ERROR: %s",
      log_msg);
  }

/* If the connection was dropped, we certainly are no longer talking TLS */
tls_in.active.sock = -1;

/* Write an SMTP response if we are expected to give one. As the default
responses are all internal, they should always fit in the buffer, but code a
warning, just in case. Note that string_vformat() still leaves a complete
string, even if it is incomplete. */

if (code && defaultrespond)
  {
  if (user_msg)
    smtp_respond(code, 3, TRUE, user_msg);
  else
    {
    gstring * g;
    va_list ap;

    va_start(ap, defaultrespond);
    g = string_vformat(NULL, TRUE, CS defaultrespond, ap);
    va_end(ap);
    smtp_printf("%s %s\r\n", FALSE, code, string_from_gstring(g));
    }
  mac_smtp_fflush();
  }
}