smtp_quit_handler(uschar ** user_msgp, uschar ** log_msgp)
{
HAD(SCH_QUIT);
incomplete_transaction_log(US"QUIT");
if (acl_smtp_quit)
  {
  int rc = acl_check(ACL_WHERE_QUIT, NULL, acl_smtp_quit, user_msgp, log_msgp);
  if (rc == ERROR)
    log_write(0, LOG_MAIN|LOG_PANIC, "ACL for QUIT returned ERROR: %s",
      *log_msgp);
  }
if (*user_msgp)
  smtp_respond(US"221", 3, TRUE, *user_msgp);
else
  smtp_printf("221 %s closing connection\r\n", FALSE, smtp_active_hostname);

#ifdef SUPPORT_TLS
tls_close(NULL, TLS_SHUTDOWN_NOWAIT);
#endif

log_write(L_smtp_connection, LOG_MAIN, "%s closed by QUIT",
  smtp_get_connection_info());
}