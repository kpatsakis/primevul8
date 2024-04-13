check_helo(uschar *s)
{
uschar *start = s;
uschar *end = s + Ustrlen(s);
BOOL yield = fl.helo_accept_junk;

/* Discard any previous helo name */

if (sender_helo_name)
  {
  store_free(sender_helo_name);
  sender_helo_name = NULL;
  }

/* Skip tests if junk is permitted. */

if (!yield)

  /* Allow the new standard form for IPv6 address literals, namely,
  [IPv6:....], and because someone is bound to use it, allow an equivalent
  IPv4 form. Allow plain addresses as well. */

  if (*s == '[')
    {
    if (end[-1] == ']')
      {
      end[-1] = 0;
      if (strncmpic(s, US"[IPv6:", 6) == 0)
        yield = (string_is_ip_address(s+6, NULL) == 6);
      else if (strncmpic(s, US"[IPv4:", 6) == 0)
        yield = (string_is_ip_address(s+6, NULL) == 4);
      else
        yield = (string_is_ip_address(s+1, NULL) != 0);
      end[-1] = ']';
      }
    }

  /* Non-literals must be alpha, dot, hyphen, plus any non-valid chars
  that have been configured (usually underscore - sigh). */

  else if (*s)
    for (yield = TRUE; *s; s++)
      if (!isalnum(*s) && *s != '.' && *s != '-' &&
          Ustrchr(helo_allow_chars, *s) == NULL)
        {
        yield = FALSE;
        break;
        }

/* Save argument if OK */

if (yield) sender_helo_name = string_copy_malloc(start);
return yield;
}