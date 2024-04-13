normalize_line (guint8 * buffer)
{
  while (*buffer) {
    if (g_ascii_isspace (*buffer)) {
      guint8 *tmp;

      *buffer++ = ' ';
      for (tmp = buffer; g_ascii_isspace (*tmp); tmp++) {
      }
      if (buffer != tmp)
        memmove (buffer, tmp, strlen ((gchar *) tmp) + 1);
    } else {
      buffer++;
    }
  }
}