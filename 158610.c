debug_printf_indent(const char * format, ...)
{
va_list ap;
va_start(ap, format);
debug_vprintf(acl_level + expand_level, format, ap);
va_end(ap);
}