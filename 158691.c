smtp_log_tls_fail(uschar * errstr)
{
uschar * conn_info = smtp_get_connection_info();

if (Ustrncmp(conn_info, US"SMTP ", 5) == 0) conn_info += 5;
/* I'd like to get separated H= here, but too hard for now */

log_write(0, LOG_MAIN, "TLS error on %s %s", conn_info, errstr);
return FALSE;
}