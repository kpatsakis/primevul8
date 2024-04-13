deliver_msglog(const char *format, ...)
{
va_list ap;
if (!message_logs) return;
va_start(ap, format);
vfprintf(message_log, format, ap);
fflush(message_log);
va_end(ap);
}