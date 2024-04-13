smtp_log_no_mail(void)
{
int i;
uschar * sep, * s;
gstring * g = NULL;

if (smtp_mailcmd_count > 0 || !LOGGING(smtp_no_mail))
  return;

if (sender_host_authenticated)
  {
  g = string_append(g, 2, US" A=", sender_host_authenticated);
  if (authenticated_id) g = string_append(g, 2, US":", authenticated_id);
  }

#ifdef SUPPORT_TLS
g = s_tlslog(g);
#endif

sep = smtp_connection_had[SMTP_HBUFF_SIZE-1] != SCH_NONE ?  US" C=..." : US" C=";

for (i = smtp_ch_index; i < SMTP_HBUFF_SIZE; i++)
  if (smtp_connection_had[i] != SCH_NONE)
    {
    g = string_append(g, 2, sep, smtp_names[smtp_connection_had[i]]);
    sep = US",";
    }

for (i = 0; i < smtp_ch_index; i++)
  {
  g = string_append(g, 2, sep, smtp_names[smtp_connection_had[i]]);
  sep = US",";
  }

if (!(s = string_from_gstring(g))) s = US"";

log_write(0, LOG_MAIN, "no MAIL in %sSMTP connection from %s D=%s%s",
  f.tcp_in_fastopen ? f.tcp_in_fastopen_data ? US"TFO* " : US"TFO " : US"",
  host_and_ident(FALSE), string_timesince(&smtp_connection_start), s);
}