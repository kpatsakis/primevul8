set_file_path(void)
{
int sep = ':';              /* Fixed separator - outside use */
uschar *t;
const uschar *tt = US LOG_FILE_PATH;
while ((t = string_nextinlist(&tt, &sep, log_buffer, LOG_BUFFER_SIZE)))
  {
  if (Ustrcmp(t, "syslog") == 0 || t[0] == 0) continue;
  file_path = string_copy(t);
  break;
  }
}