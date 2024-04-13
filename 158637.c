acl_udpsend(const uschar *arg, uschar **log_msgptr)
{
int sep = 0;
uschar *hostname;
uschar *portstr;
uschar *portend;
host_item *h;
int portnum;
int len;
int r, s;
uschar * errstr;

hostname = string_nextinlist(&arg, &sep, NULL, 0);
portstr = string_nextinlist(&arg, &sep, NULL, 0);

if (!hostname)
  {
  *log_msgptr = US"missing destination host in \"udpsend\" modifier";
  return ERROR;
  }
if (!portstr)
  {
  *log_msgptr = US"missing destination port in \"udpsend\" modifier";
  return ERROR;
  }
if (!arg)
  {
  *log_msgptr = US"missing datagram payload in \"udpsend\" modifier";
  return ERROR;
  }
portnum = Ustrtol(portstr, &portend, 10);
if (*portend != '\0')
  {
  *log_msgptr = US"bad destination port in \"udpsend\" modifier";
  return ERROR;
  }

/* Make a single-item host list. */
h = store_get(sizeof(host_item));
memset(h, 0, sizeof(host_item));
h->name = hostname;
h->port = portnum;
h->mx = MX_NONE;

if (string_is_ip_address(hostname, NULL))
  h->address = hostname, r = HOST_FOUND;
else
  r = host_find_byname(h, NULL, 0, NULL, FALSE);
if (r == HOST_FIND_FAILED || r == HOST_FIND_AGAIN)
  {
  *log_msgptr = US"DNS lookup failed in \"udpsend\" modifier";
  return DEFER;
  }

HDEBUG(D_acl)
  debug_printf_indent("udpsend [%s]:%d %s\n", h->address, portnum, arg);

/*XXX this could better use sendto */
r = s = ip_connectedsocket(SOCK_DGRAM, h->address, portnum, portnum,
		1, NULL, &errstr, NULL);
if (r < 0) goto defer;
len = Ustrlen(arg);
r = send(s, arg, len, 0);
if (r < 0)
  {
  errstr = US strerror(errno);
  close(s);
  goto defer;
  }
close(s);
if (r < len)
  {
  *log_msgptr =
    string_sprintf("\"udpsend\" truncated from %d to %d octets", len, r);
  return DEFER;
  }

HDEBUG(D_acl)
  debug_printf_indent("udpsend %d bytes\n", r);

return OK;

defer:
*log_msgptr = string_sprintf("\"udpsend\" failed: %s", errstr);
return DEFER;
}