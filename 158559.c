smtp_command_sigterm_exit(void)
{
log_write(0, LOG_MAIN, "%s closed after SIGTERM", smtp_get_connection_info());
if (smtp_batched_input)
  moan_smtp_batch(NULL, "421 SIGTERM received");  /* Does not return */
smtp_notquit_exit(US"signal-exit", US"421",
  US"%s: Service not available - closing connection", smtp_active_hostname);
exim_exit(EXIT_FAILURE, US"receiving");
}