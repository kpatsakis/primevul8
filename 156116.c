string_nextinlist(const uschar **listptr, int *separator, uschar *buffer, int buflen)
{
int sep = *separator;
const uschar *s = *listptr;
BOOL sep_is_special;

if (!s) return NULL;

/* This allows for a fixed specified separator to be an iscntrl() character,
but at the time of implementation, this is never the case. However, it's best
to be conservative. */

while (isspace(*s) && *s != sep) s++;

/* A change of separator is permitted, so look for a leading '<' followed by an
allowed character. */

if (sep <= 0)
  {
  if (*s == '<' && (ispunct(s[1]) || iscntrl(s[1])))
    {
    sep = s[1];
    if (*++s) ++s;
    while (isspace(*s) && *s != sep) s++;
    }
  else
    sep = sep ? -sep : ':';
  *separator = sep;
  }

/* An empty string has no list elements */

if (!*s) return NULL;

/* Note whether whether or not the separator is an iscntrl() character. */

sep_is_special = iscntrl(sep);

/* Handle the case when a buffer is provided. */

if (buffer)
  {
  int p = 0;
  for (; *s; s++)
    {
    if (*s == sep && (*(++s) != sep || sep_is_special)) break;
    if (p < buflen - 1) buffer[p++] = *s;
    }
  while (p > 0 && isspace(buffer[p-1])) p--;
  buffer[p] = '\0';
  }

/* Handle the case when a buffer is not provided. */

else
  {
  const uschar *ss;
  gstring * g = NULL;

  /* We know that *s != 0 at this point. However, it might be pointing to a
  separator, which could indicate an empty string, or (if an ispunct()
  character) could be doubled to indicate a separator character as data at the
  start of a string. Avoid getting working memory for an empty item. */

  if (*s == sep)
    {
    s++;
    if (*s != sep || sep_is_special)
      {
      *listptr = s;
      return string_copy(US"");
      }
    }

  /* Not an empty string; the first character is guaranteed to be a data
  character. */

  for (;;)
    {
    for (ss = s + 1; *ss && *ss != sep; ss++) ;
    g = string_catn(g, s, ss-s);
    s = ss;
    if (!*s || *++s != sep || sep_is_special) break;
    }
  while (g->ptr > 0 && isspace(g->s[g->ptr-1])) g->ptr--;
  buffer = string_from_gstring(g);
  gstring_reset_unused(g);
  }

/* Update the current pointer and return the new string */

*listptr = s;
return buffer;
}