string_fmt_append(gstring * g, const char *format, ...)
{
va_list ap;
va_start(ap, format);
g = string_vformat(g, TRUE, format, ap);
va_end(ap);
return g;
}