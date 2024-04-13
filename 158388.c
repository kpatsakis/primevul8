void _debug(FILE *debug_file, const char *file, int line, const char *func, const char *fmt, ...) {
  va_list ap;
#ifdef __linux__
  unsigned int size;
  char buffer[BUFSIZE];
  char *out;

  size = (unsigned int)snprintf(NULL, 0, DEBUG_STR, file, line, func);
  va_start(ap, fmt);
  size += (unsigned int)vsnprintf(NULL, 0, fmt, ap);
  va_end(ap);
  va_start(ap, fmt);
  if (size < (BUFSIZE - 1)) {
    out = buffer;
  }
  else {
    out = malloc(size);
  }

  if (out) {
    size = (unsigned int)sprintf(out, DEBUG_STR, file, line, func);
    vsprintf(&out[size], fmt, ap);
    va_end(ap);
  }
  else {
    out = buffer;
    sprintf(out, "debug(pam_u2f): malloc failed when trying to log\n");
  }

  if (debug_file == (FILE *)-1) {
    syslog(LOG_AUTHPRIV | LOG_DEBUG, "%s", out);
  }
  else {
    fprintf(debug_file, "%s\n", out);
  }

  if (out != buffer) {
    free(out);
  }
#else /* Windows, MAC */
  va_start(ap, fmt);
  fprintf(debug_file, DEBUG_STR, file, line, func );
  vfprintf(debug_file, fmt, ap);
  fprintf(debug_file, "\n");
  va_end(ap);
#endif /* __linux__ */
}