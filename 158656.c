next_emf(FILE *f, uschar *which)
{
uschar *yield;
gstring * para;
uschar buffer[256];

if (!f) return NULL;

if (!Ufgets(buffer, sizeof(buffer), f) || Ustrcmp(buffer, "****\n") == 0)
  return NULL;

para = string_get(256);
for (;;)
  {
  para = string_cat(para, buffer);
  if (!Ufgets(buffer, sizeof(buffer), f) || Ustrcmp(buffer, "****\n") == 0)
    break;
  }
if ((yield = expand_string(string_from_gstring(para))))
  return yield;

log_write(0, LOG_MAIN|LOG_PANIC, "Failed to expand string from "
  "bounce_message_file or warn_message_file (%s): %s", which,
  expand_string_message);
return NULL;
}