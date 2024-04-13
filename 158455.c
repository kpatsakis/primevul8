host_name_lookup_byaddr(void)
{
int len;
uschar *s, *t;
struct hostent *hosts;
struct in_addr addr;
unsigned long time_msec = 0;	/* init to quieten dumb static analysis */

if (slow_lookup_log) time_msec = get_time_in_ms();

/* Lookup on IPv6 system */

#if HAVE_IPV6
if (Ustrchr(sender_host_address, ':') != NULL)
  {
  struct in6_addr addr6;
  if (inet_pton(AF_INET6, CS sender_host_address, &addr6) != 1)
    log_write(0, LOG_MAIN|LOG_PANIC_DIE, "unable to parse \"%s\" as an "
      "IPv6 address", sender_host_address);
  #if HAVE_GETIPNODEBYADDR
  hosts = getipnodebyaddr(CS &addr6, sizeof(addr6), AF_INET6, &h_errno);
  #else
  hosts = gethostbyaddr(CS &addr6, sizeof(addr6), AF_INET6);
  #endif
  }
else
  {
  if (inet_pton(AF_INET, CS sender_host_address, &addr) != 1)
    log_write(0, LOG_MAIN|LOG_PANIC_DIE, "unable to parse \"%s\" as an "
      "IPv4 address", sender_host_address);
  #if HAVE_GETIPNODEBYADDR
  hosts = getipnodebyaddr(CS &addr, sizeof(addr), AF_INET, &h_errno);
  #else
  hosts = gethostbyaddr(CS &addr, sizeof(addr), AF_INET);
  #endif
  }

/* Do lookup on IPv4 system */

#else
addr.s_addr = (S_ADDR_TYPE)inet_addr(CS sender_host_address);
hosts = gethostbyaddr(CS(&addr), sizeof(addr), AF_INET);
#endif

if (  slow_lookup_log
   && (time_msec = get_time_in_ms() - time_msec) > slow_lookup_log
   )
  log_long_lookup(US"name", sender_host_address, time_msec);

/* Failed to look up the host. */

if (hosts == NULL)
  {
  HDEBUG(D_host_lookup) debug_printf("IP address lookup failed: h_errno=%d\n",
    h_errno);
  return (h_errno == TRY_AGAIN || h_errno == NO_RECOVERY) ? DEFER : FAIL;
  }

/* It seems there are some records in the DNS that yield an empty name. We
treat this as non-existent. In some operating systems, this is returned as an
empty string; in others as a single dot. */

if (hosts->h_name == NULL || hosts->h_name[0] == 0 || hosts->h_name[0] == '.')
  {
  HDEBUG(D_host_lookup) debug_printf("IP address lookup yielded an empty name: "
    "treated as non-existent host name\n");
  return FAIL;
  }

/* Copy and lowercase the name, which is in static storage in many systems.
Put it in permanent memory. */

s = US hosts->h_name;
len = Ustrlen(s) + 1;
t = sender_host_name = store_get_perm(len);
while (*s != 0) *t++ = tolower(*s++);
*t = 0;

/* If the host has aliases, build a copy of the alias list */

if (hosts->h_aliases != NULL)
  {
  int count = 1;
  uschar **aliases, **ptr;
  for (aliases = USS hosts->h_aliases; *aliases != NULL; aliases++) count++;
  ptr = sender_host_aliases = store_get_perm(count * sizeof(uschar *));
  for (aliases = USS hosts->h_aliases; *aliases != NULL; aliases++)
    {
    uschar *s = *aliases;
    int len = Ustrlen(s) + 1;
    uschar *t = *ptr++ = store_get_perm(len);
    while (*s != 0) *t++ = tolower(*s++);
    *t = 0;
    }
  *ptr = NULL;
  }

return OK;
}