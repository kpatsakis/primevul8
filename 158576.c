common_error(BOOL logit, address_item *addr, int code, uschar *format, ...)
{
address_item *addr2;
addr->basic_errno = code;

if (format)
  {
  va_list ap;
  gstring * g;

  va_start(ap, format);
  g = string_vformat(NULL, TRUE, CS format, ap);
  va_end(ap);
  addr->message = string_from_gstring(g);
  }

for (addr2 = addr->next; addr2; addr2 = addr2->next)
  {
  addr2->basic_errno = code;
  addr2->message = addr->message;
  }

if (logit) log_write(0, LOG_MAIN|LOG_PANIC, "%s", addr->message);
deliver_set_expansions(NULL);
}