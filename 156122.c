string_format(uschar * buffer, int buflen, const char * format, ...)
{
gstring g = { .size = buflen, .ptr = 0, .s = buffer }, *gp;
va_list ap;
va_start(ap, format);
gp = string_vformat(&g, FALSE, format, ap);
va_end(ap);
g.s[g.ptr] = '\0';
return !!gp;
}