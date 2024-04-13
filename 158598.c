smtp_command_timeout_exit(void)
{
log_write(L_lost_incoming_connection,
	  LOG_MAIN, "SMTP command timeout on%s connection from %s",
	  tls_in.active.sock >= 0 ? " TLS" : "", host_and_ident(FALSE));
if (smtp_batched_input)
  moan_smtp_batch(NULL, "421 SMTP command timeout"); /* Does not return */
smtp_notquit_exit(US"command-timeout", US"421",
  US"%s: SMTP command timeout - closing connection",
  smtp_active_hostname);
exim_exit(EXIT_FAILURE, US"receiving");
}