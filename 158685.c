synprot_error(int type, int code, uschar *data, uschar *errmess)
{
int yield = -1;

log_write(type, LOG_MAIN, "SMTP %s error in \"%s\" %s %s",
  (type == L_smtp_syntax_error)? "syntax" : "protocol",
  string_printing(smtp_cmd_buffer), host_and_ident(TRUE), errmess);

if (++synprot_error_count > smtp_max_synprot_errors)
  {
  yield = 1;
  log_write(0, LOG_MAIN|LOG_REJECT, "SMTP call from %s dropped: too many "
    "syntax or protocol errors (last command was \"%s\")",
    host_and_ident(FALSE), string_printing(smtp_cmd_buffer));
  }

if (code > 0)
  {
  smtp_printf("%d%c%s%s%s\r\n", FALSE, code, yield == 1 ? '-' : ' ',
    data ? data : US"", data ? US": " : US"", errmess);
  if (yield == 1)
    smtp_printf("%d Too many syntax or protocol errors\r\n", FALSE, code);
  }

return yield;
}