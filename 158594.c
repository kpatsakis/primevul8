die(uschar *s1, uschar *s2)
{
if (s1)
  {
  write_syslog(LOG_CRIT, s1);
  if (debug_file) debug_printf("%s\n", s1);
  if (log_stderr && log_stderr != debug_file)
    fprintf(log_stderr, "%s\n", s1);
  }
if (f.receive_call_bombout) receive_bomb_out(NULL, s2);  /* does not return */
if (smtp_input) smtp_closedown(s2);
exim_exit(EXIT_FAILURE, NULL);
}