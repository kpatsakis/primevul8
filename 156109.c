string_sprintf(const char *format, ...)
{
#ifdef COMPILE_UTILITY
uschar buffer[STRING_SPRINTF_BUFFER_SIZE];
gstring g = { .size = STRING_SPRINTF_BUFFER_SIZE, .ptr = 0, .s = buffer };
gstring * gp = &g;
#else
gstring * gp = string_get(STRING_SPRINTF_BUFFER_SIZE);
#endif
gstring * gp2;
va_list ap;

va_start(ap, format);
gp2 = string_vformat(gp, FALSE, format, ap);
gp->s[gp->ptr] = '\0';
va_end(ap);

if (!gp2)
  log_write(0, LOG_MAIN|LOG_PANIC_DIE,
    "string_sprintf expansion was longer than %d; format string was (%s)\n"
    "expansion started '%.32s'",
    gp->size, format, gp->s);

#ifdef COMPILE_UTILITY
return string_copy(gp->s);
#else
gstring_reset_unused(gp);
return gp->s;
#endif
}