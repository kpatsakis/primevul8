ratelimit_error(uschar **log_msgptr, const char *format, ...)
{
va_list ap;
gstring * g =
  string_cat(NULL, US"error in arguments to \"ratelimit\" condition: ");

va_start(ap, format);
g = string_vformat(g, TRUE, format, ap);
va_end(ap);

gstring_reset_unused(g);
*log_msgptr = string_from_gstring(g);
return ERROR;
}