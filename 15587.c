zlog (struct zlog *zl, int priority, const char *format, ...)
{
  va_list args;

  va_start(args, format);
  vzlog (zl, priority, format, args);
  va_end (args);
}