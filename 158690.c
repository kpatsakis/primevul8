get_stdinput(char *(*fn_readline)(const char *), void(*fn_addhist)(const char *))
{
int i;
gstring * g = NULL;

if (!fn_readline) { printf("> "); fflush(stdout); }

for (i = 0;; i++)
  {
  uschar buffer[1024];
  uschar *p, *ss;

  #ifdef USE_READLINE
  char *readline_line = NULL;
  if (fn_readline != NULL)
    {
    if ((readline_line = fn_readline((i > 0)? "":"> ")) == NULL) break;
    if (*readline_line != 0 && fn_addhist != NULL) fn_addhist(readline_line);
    p = US readline_line;
    }
  else
  #endif

  /* readline() not in use */

    {
    if (Ufgets(buffer, sizeof(buffer), stdin) == NULL) break;
    p = buffer;
    }

  /* Handle the line */

  ss = p + (int)Ustrlen(p);
  while (ss > p && isspace(ss[-1])) ss--;

  if (i > 0)
    {
    while (p < ss && isspace(*p)) p++;   /* leading space after cont */
    }

  g = string_catn(g, p, ss - p);

  #ifdef USE_READLINE
  if (fn_readline) free(readline_line);
  #endif

  /* g can only be NULL if ss==p */
  if (ss == p || g->s[g->ptr-1] != '\\')
    break;

  --g->ptr;
  (void) string_from_gstring(g);
  }

if (!g) printf("\n");
return string_from_gstring(g);
}