string_is_ip_address(const uschar *s, int *maskptr)
{
int i;
int yield = 4;

/* If an optional mask is permitted, check for it. If found, pass back the
offset. */

if (maskptr)
  {
  const uschar *ss = s + Ustrlen(s);
  *maskptr = 0;
  if (s != ss && isdigit(*(--ss)))
    {
    while (ss > s && isdigit(ss[-1])) ss--;
    if (ss > s && *(--ss) == '/') *maskptr = ss - s;
    }
  }

/* A colon anywhere in the string => IPv6 address */

if (Ustrchr(s, ':') != NULL)
  {
  BOOL had_double_colon = FALSE;
  BOOL v4end = FALSE;
  int count = 0;

  yield = 6;

  /* An IPv6 address must start with hex digit or double colon. A single
  colon is invalid. */

  if (*s == ':' && *(++s) != ':') return 0;

  /* Now read up to 8 components consisting of up to 4 hex digits each. There
  may be one and only one appearance of double colon, which implies any number
  of binary zero bits. The number of preceding components is held in count. */

  for (count = 0; count < 8; count++)
    {
    /* If the end of the string is reached before reading 8 components, the
    address is valid provided a double colon has been read. This also applies
    if we hit the / that introduces a mask or the % that introduces the
    interface specifier (scope id) of a link-local address. */

    if (*s == 0 || *s == '%' || *s == '/') return had_double_colon ? yield : 0;

    /* If a component starts with an additional colon, we have hit a double
    colon. This is permitted to appear once only, and counts as at least
    one component. The final component may be of this form. */

    if (*s == ':')
      {
      if (had_double_colon) return 0;
      had_double_colon = TRUE;
      s++;
      continue;
      }

    /* If the remainder of the string contains a dot but no colons, we
    can expect a trailing IPv4 address. This is valid if either there has
    been no double-colon and this is the 7th component (with the IPv4 address
    being the 7th & 8th components), OR if there has been a double-colon
    and fewer than 6 components. */

    if (Ustrchr(s, ':') == NULL && Ustrchr(s, '.') != NULL)
      {
      if ((!had_double_colon && count != 6) ||
          (had_double_colon && count > 6)) return 0;
      v4end = TRUE;
      yield = 6;
      break;
      }

    /* Check for at least one and not more than 4 hex digits for this
    component. */

    if (!isxdigit(*s++)) return 0;
    if (isxdigit(*s) && isxdigit(*(++s)) && isxdigit(*(++s))) s++;

    /* If the component is terminated by colon and there is more to
    follow, skip over the colon. If there is no more to follow the address is
    invalid. */

    if (*s == ':' && *(++s) == 0) return 0;
    }

  /* If about to handle a trailing IPv4 address, drop through. Otherwise
  all is well if we are at the end of the string or at the mask or at a percent
  sign, which introduces the interface specifier (scope id) of a link local
  address. */

  if (!v4end)
    return (*s == 0 || *s == '%' ||
           (*s == '/' && maskptr != NULL && *maskptr != 0))? yield : 0;
  }

/* Test for IPv4 address, which may be the tail-end of an IPv6 address. */

for (i = 0; i < 4; i++)
  {
  long n;
  uschar * end;

  if (i != 0 && *s++ != '.') return 0;
  n = strtol(CCS s, CSS &end, 10);
  if (n > 255 || n < 0 || end <= s || end > s+3) return 0;
  s = end;
  }

return !*s || (*s == '/' && maskptr && *maskptr != 0) ? yield : 0;
}