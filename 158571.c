host_address_extract_port(uschar *address)
{
int port = 0;
uschar *endptr;

/* Handle the "bracketed with colon on the end" format */

if (*address == '[')
  {
  uschar *rb = address + 1;
  while (*rb != 0 && *rb != ']') rb++;
  if (*rb++ == 0) return 0;        /* Missing ]; leave invalid address */
  if (*rb == ':')
    {
    port = Ustrtol(rb + 1, &endptr, 10);
    if (*endptr != 0) return 0;    /* Invalid port; leave invalid address */
    }
  else if (*rb != 0) return 0;     /* Bad syntax; leave invalid address */
  memmove(address, address + 1, rb - address - 2);
  rb[-2] = 0;
  }

/* Handle the "dot on the end" format */

else
  {
  int skip = -3;                   /* Skip 3 dots in IPv4 addresses */
  address--;
  while (*(++address) != 0)
    {
    int ch = *address;
    if (ch == ':') skip = 0;       /* Skip 0 dots in IPv6 addresses */
      else if (ch == '.' && skip++ >= 0) break;
    }
  if (*address == 0) return 0;
  port = Ustrtol(address + 1, &endptr, 10);
  if (*endptr != 0) return 0;      /* Invalid port; leave invalid address */
  *address = 0;
  }

return port;
}