macros_trusted(BOOL opt_D_used)
{
#ifdef WHITELIST_D_MACROS
macro_item *m;
uschar *whitelisted, *end, *p, **whites, **w;
int white_count, i, n;
size_t len;
BOOL prev_char_item, found;
#endif

if (!opt_D_used)
  return TRUE;
#ifndef WHITELIST_D_MACROS
return FALSE;
#else

/* We only trust -D overrides for some invoking users:
root, the exim run-time user, the optional config owner user.
I don't know why config-owner would be needed, but since they can own the
config files anyway, there's no security risk to letting them override -D. */
if ( ! ((real_uid == root_uid)
     || (real_uid == exim_uid)
#ifdef CONFIGURE_OWNER
     || (real_uid == config_uid)
#endif
   ))
  {
  debug_printf("macros_trusted rejecting macros for uid %d\n", (int) real_uid);
  return FALSE;
  }

/* Get a list of macros which are whitelisted */
whitelisted = string_copy_malloc(US WHITELIST_D_MACROS);
prev_char_item = FALSE;
white_count = 0;
for (p = whitelisted; *p != '\0'; ++p)
  {
  if (*p == ':' || isspace(*p))
    {
    *p = '\0';
    if (prev_char_item)
      ++white_count;
    prev_char_item = FALSE;
    continue;
    }
  if (!prev_char_item)
    prev_char_item = TRUE;
  }
end = p;
if (prev_char_item)
  ++white_count;
if (!white_count)
  return FALSE;
whites = store_malloc(sizeof(uschar *) * (white_count+1));
for (p = whitelisted, i = 0; (p != end) && (i < white_count); ++p)
  {
  if (*p != '\0')
    {
    whites[i++] = p;
    if (i == white_count)
      break;
    while (*p != '\0' && p < end)
      ++p;
    }
  }
whites[i] = NULL;

/* The list of commandline macros should be very short.
Accept the N*M complexity. */
for (m = macros_user; m; m = m->next) if (m->command_line)
  {
  found = FALSE;
  for (w = whites; *w; ++w)
    if (Ustrcmp(*w, m->name) == 0)
      {
      found = TRUE;
      break;
      }
  if (!found)
    return FALSE;
  if (!m->replacement)
    continue;
  if ((len = m->replen) == 0)
    continue;
  n = pcre_exec(regex_whitelisted_macro, NULL, CS m->replacement, len,
   0, PCRE_EOPT, NULL, 0);
  if (n < 0)
    {
    if (n != PCRE_ERROR_NOMATCH)
      debug_printf("macros_trusted checking %s returned %d\n", m->name, n);
    return FALSE;
    }
  }
DEBUG(D_any) debug_printf("macros_trusted overridden to true by whitelisting\n");
return TRUE;
#endif
}