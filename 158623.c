log_long_lookup(const uschar * type, const uschar * data, unsigned long msec)
{
log_write(0, LOG_MAIN, "Long %s lookup for '%s': %lu msec",
  type, data, msec);
}