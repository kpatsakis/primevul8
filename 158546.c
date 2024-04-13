smtp_data_sigint_exit(void)
{
log_write(0, LOG_MAIN, "%s closed after %s",
  smtp_get_connection_info(), had_data_sigint == SIGTERM ? "SIGTERM":"SIGINT");
receive_bomb_out(US"signal-exit",
  US"Service not available - SIGTERM or SIGINT received");
/* Does not return */
}