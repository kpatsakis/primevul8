host_aton(const uschar *address, int *bin)
{
int x[4];
int v4offset = 0;

/* Handle IPv6 address, which may end with an IPv4 address. It may also end
with a "scope", introduced by a percent sign. This code is NOT enclosed in #if
HAVE_IPV6 in order that IPv6 addresses are recognized even if IPv6 is not
supported. */

if (Ustrchr(address, ':') != NULL)
  {
  const uschar *p = address;
  const uschar *component[8];
  BOOL ipv4_ends = FALSE;
  int ci = 0;
  int nulloffset = 0;
  int v6count = 8;
  int i;

  /* If the address starts with a colon, it will start with two colons.
  Just lose the first one, which will leave a null first component. */

  if (*p == ':') p++;

  /* Split the address into components separated by colons. The input address
  is supposed to be checked for syntax. There was a case where this was
  overlooked; to guard against that happening again, check here and crash if
  there are too many components. */

  while (*p != 0 && *p != '%')
    {
    int len = Ustrcspn(p, ":%");
    if (len == 0) nulloffset = ci;
    if (ci > 7) log_write(0, LOG_MAIN|LOG_PANIC_DIE,
      "Internal error: invalid IPv6 address \"%s\" passed to host_aton()",
      address);
    component[ci++] = p;
    p += len;
    if (*p == ':') p++;
    }

  /* If the final component contains a dot, it is a trailing v4 address.
  As the syntax is known to be checked, just set up for a trailing
  v4 address and restrict the v6 part to 6 components. */

  if (Ustrchr(component[ci-1], '.') != NULL)
    {
    address = component[--ci];
    ipv4_ends = TRUE;
    v4offset = 3;
    v6count = 6;
    }

  /* If there are fewer than 6 or 8 components, we have to insert some
  more empty ones in the middle. */

  if (ci < v6count)
    {
    int insert_count = v6count - ci;
    for (i = v6count-1; i > nulloffset + insert_count; i--)
      component[i] = component[i - insert_count];
    while (i > nulloffset) component[i--] = US"";
    }

  /* Now turn the components into binary in pairs and bung them
  into the vector of ints. */

  for (i = 0; i < v6count; i += 2)
    bin[i/2] = (Ustrtol(component[i], NULL, 16) << 16) +
      Ustrtol(component[i+1], NULL, 16);

  /* If there was no terminating v4 component, we are done. */

  if (!ipv4_ends) return 4;
  }

/* Handle IPv4 address */

(void)sscanf(CS address, "%d.%d.%d.%d", x, x+1, x+2, x+3);
bin[v4offset] = ((uint)x[0] << 24) + (x[1] << 16) + (x[2] << 8) + x[3];
return v4offset+1;
}