setup_proxy_protocol_host()
{
union {
  struct {
    uschar line[108];
  } v1;
  struct {
    uschar sig[12];
    uint8_t ver_cmd;
    uint8_t fam;
    uint16_t len;
    union {
      struct { /* TCP/UDP over IPv4, len = 12 */
        uint32_t src_addr;
        uint32_t dst_addr;
        uint16_t src_port;
        uint16_t dst_port;
      } ip4;
      struct { /* TCP/UDP over IPv6, len = 36 */
        uint8_t  src_addr[16];
        uint8_t  dst_addr[16];
        uint16_t src_port;
        uint16_t dst_port;
      } ip6;
      struct { /* AF_UNIX sockets, len = 216 */
        uschar   src_addr[108];
        uschar   dst_addr[108];
      } unx;
    } addr;
  } v2;
} hdr;

/* Temp variables used in PPv2 address:port parsing */
uint16_t tmpport;
char tmpip[INET_ADDRSTRLEN];
struct sockaddr_in tmpaddr;
char tmpip6[INET6_ADDRSTRLEN];
struct sockaddr_in6 tmpaddr6;

/* We can't read "all data until end" because while SMTP is
server-speaks-first, the TLS handshake is client-speaks-first, so for
TLS-on-connect ports the proxy protocol header will usually be immediately
followed by a TLS handshake, and with N TLS libraries, we can't reliably
reinject data for reading by those.  So instead we first read "enough to be
safely read within the header, and figure out how much more to read".
For v1 we will later read to the end-of-line, for v2 we will read based upon
the stated length.

The v2 sig is 12 octets, and another 4 gets us the length, so we know how much
data is needed total.  For v1, where the line looks like:
PROXY TCPn L3src L3dest SrcPort DestPort \r\n

However, for v1 there's also `PROXY UNKNOWN\r\n` which is only 15 octets.
We seem to support that.  So, if we read 14 octets then we can tell if we're
v2 or v1.  If we're v1, we can continue reading as normal.

If we're v2, we can't slurp up the entire header.  We need the length in the
15th & 16th octets, then to read everything after that.

So to safely handle v1 and v2, with client-sent-first supported correctly,
we have to do a minimum of 3 read calls, not 1.  Eww.
*/

#define PROXY_INITIAL_READ 14
#define PROXY_V2_HEADER_SIZE 16
#if PROXY_INITIAL_READ > PROXY_V2_HEADER_SIZE
# error Code bug in sizes of data to read for proxy usage
#endif

int get_ok = 0;
int size, ret;
int fd = fileno(smtp_in);
const char v2sig[12] = "\x0D\x0A\x0D\x0A\x00\x0D\x0A\x51\x55\x49\x54\x0A";
uschar * iptype;  /* To display debug info */
struct timeval tv;
struct timeval tvtmp;
socklen_t vslen = sizeof(struct timeval);
BOOL yield = FALSE;

/* Save current socket timeout values */
get_ok = getsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, CS &tvtmp, &vslen);

/* Proxy Protocol host must send header within a short time
(default 3 seconds) or it's considered invalid */
tv.tv_sec  = PROXY_NEGOTIATION_TIMEOUT_SEC;
tv.tv_usec = PROXY_NEGOTIATION_TIMEOUT_USEC;
if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, CS &tv, sizeof(tv)) < 0)
  goto bad;

do
  {
  /* The inbound host was declared to be a Proxy Protocol host, so
  don't do a PEEK into the data, actually slurp up enough to be
  "safe". Can't take it all because TLS-on-connect clients follow
  immediately with TLS handshake. */
  ret = recv(fd, &hdr, PROXY_INITIAL_READ, 0);
  }
  while (ret == -1 && errno == EINTR);

if (ret == -1)
  goto proxyfail;

/* For v2, handle reading the length, and then the rest. */
if ((ret == PROXY_INITIAL_READ) && (memcmp(&hdr.v2, v2sig, sizeof(v2sig)) == 0))
  {
  int retmore;
  uint8_t ver;

  /* First get the length fields. */
  do
    {
    retmore = recv(fd, (uschar*)&hdr + ret, PROXY_V2_HEADER_SIZE - PROXY_INITIAL_READ, 0);
    } while (retmore == -1 && errno == EINTR);
  if (retmore == -1)
    goto proxyfail;
  ret += retmore;

  ver = (hdr.v2.ver_cmd & 0xf0) >> 4;

  /* May 2014: haproxy combined the version and command into one byte to
  allow two full bytes for the length field in order to proxy SSL
  connections.  SSL Proxy is not supported in this version of Exim, but
  must still separate values here. */

  if (ver != 0x02)
    {
    DEBUG(D_receive) debug_printf("Invalid Proxy Protocol version: %d\n", ver);
    goto proxyfail;
    }

  /* The v2 header will always be 16 bytes per the spec. */
  size = 16 + ntohs(hdr.v2.len);
  DEBUG(D_receive) debug_printf("Detected PROXYv2 header, size %d (limit %d)\n",
      size, (int)sizeof(hdr));

  /* We should now have 16 octets (PROXY_V2_HEADER_SIZE), and we know the total
  amount that we need.  Double-check that the size is not unreasonable, then
  get the rest. */
  if (size > sizeof(hdr))
    {
    DEBUG(D_receive) debug_printf("PROXYv2 header size unreasonably large; security attack?\n");
    goto proxyfail;
    }

  do
    {
    do
      {
      retmore = recv(fd, (uschar*)&hdr + ret, size-ret, 0);
      } while (retmore == -1 && errno == EINTR);
    if (retmore == -1)
      goto proxyfail;
    ret += retmore;
    DEBUG(D_receive) debug_printf("PROXYv2: have %d/%d required octets\n", ret, size);
    } while (ret < size);

  } /* end scope for getting rest of data for v2 */

/* At this point: if PROXYv2, we've read the exact size required for all data;
if PROXYv1 then we've read "less than required for any valid line" and should
read the rest". */

if (ret >= 16 && memcmp(&hdr.v2, v2sig, 12) == 0)
  {
  uint8_t cmd = (hdr.v2.ver_cmd & 0x0f);

  switch (cmd)
    {
    case 0x01: /* PROXY command */
      switch (hdr.v2.fam)
        {
        case 0x11:  /* TCPv4 address type */
          iptype = US"IPv4";
          tmpaddr.sin_addr.s_addr = hdr.v2.addr.ip4.src_addr;
          inet_ntop(AF_INET, &tmpaddr.sin_addr, CS &tmpip, sizeof(tmpip));
          if (!string_is_ip_address(US tmpip, NULL))
            {
            DEBUG(D_receive) debug_printf("Invalid %s source IP\n", iptype);
            goto proxyfail;
            }
          proxy_local_address = sender_host_address;
          sender_host_address = string_copy(US tmpip);
          tmpport             = ntohs(hdr.v2.addr.ip4.src_port);
          proxy_local_port    = sender_host_port;
          sender_host_port    = tmpport;
          /* Save dest ip/port */
          tmpaddr.sin_addr.s_addr = hdr.v2.addr.ip4.dst_addr;
          inet_ntop(AF_INET, &tmpaddr.sin_addr, CS &tmpip, sizeof(tmpip));
          if (!string_is_ip_address(US tmpip, NULL))
            {
            DEBUG(D_receive) debug_printf("Invalid %s dest port\n", iptype);
            goto proxyfail;
            }
          proxy_external_address = string_copy(US tmpip);
          tmpport              = ntohs(hdr.v2.addr.ip4.dst_port);
          proxy_external_port  = tmpport;
          goto done;
        case 0x21:  /* TCPv6 address type */
          iptype = US"IPv6";
          memmove(tmpaddr6.sin6_addr.s6_addr, hdr.v2.addr.ip6.src_addr, 16);
          inet_ntop(AF_INET6, &tmpaddr6.sin6_addr, CS &tmpip6, sizeof(tmpip6));
          if (!string_is_ip_address(US tmpip6, NULL))
            {
            DEBUG(D_receive) debug_printf("Invalid %s source IP\n", iptype);
            goto proxyfail;
            }
          proxy_local_address = sender_host_address;
          sender_host_address = string_copy(US tmpip6);
          tmpport             = ntohs(hdr.v2.addr.ip6.src_port);
          proxy_local_port    = sender_host_port;
          sender_host_port    = tmpport;
          /* Save dest ip/port */
          memmove(tmpaddr6.sin6_addr.s6_addr, hdr.v2.addr.ip6.dst_addr, 16);
          inet_ntop(AF_INET6, &tmpaddr6.sin6_addr, CS &tmpip6, sizeof(tmpip6));
          if (!string_is_ip_address(US tmpip6, NULL))
            {
            DEBUG(D_receive) debug_printf("Invalid %s dest port\n", iptype);
            goto proxyfail;
            }
          proxy_external_address = string_copy(US tmpip6);
          tmpport              = ntohs(hdr.v2.addr.ip6.dst_port);
          proxy_external_port  = tmpport;
          goto done;
        default:
          DEBUG(D_receive)
            debug_printf("Unsupported PROXYv2 connection type: 0x%02x\n",
                         hdr.v2.fam);
          goto proxyfail;
        }
      /* Unsupported protocol, keep local connection address */
      break;
    case 0x00: /* LOCAL command */
      /* Keep local connection address for LOCAL */
      iptype = US"local";
      break;
    default:
      DEBUG(D_receive)
        debug_printf("Unsupported PROXYv2 command: 0x%x\n", cmd);
      goto proxyfail;
    }
  }
else if (ret >= 8 && memcmp(hdr.v1.line, "PROXY", 5) == 0)
  {
  uschar *p;
  uschar *end;
  uschar *sp;     /* Utility variables follow */
  int     tmp_port;
  int     r2;
  char   *endc;

  /* get the rest of the line */
  r2 = swallow_until_crlf(fd, (uschar*)&hdr, ret, sizeof(hdr)-ret);
  if (r2 == -1)
    goto proxyfail;
  ret += r2;

  p = string_copy(hdr.v1.line);
  end = memchr(p, '\r', ret - 1);

  if (!end || (end == (uschar*)&hdr + ret) || end[1] != '\n')
    {
    DEBUG(D_receive) debug_printf("Partial or invalid PROXY header\n");
    goto proxyfail;
    }
  *end = '\0'; /* Terminate the string */
  size = end + 2 - p; /* Skip header + CRLF */
  DEBUG(D_receive) debug_printf("Detected PROXYv1 header\n");
  DEBUG(D_receive) debug_printf("Bytes read not within PROXY header: %d\n", ret - size);
  /* Step through the string looking for the required fields. Ensure
  strict adherence to required formatting, exit for any error. */
  p += 5;
  if (!isspace(*(p++)))
    {
    DEBUG(D_receive) debug_printf("Missing space after PROXY command\n");
    goto proxyfail;
    }
  if (!Ustrncmp(p, CCS"TCP4", 4))
    iptype = US"IPv4";
  else if (!Ustrncmp(p,CCS"TCP6", 4))
    iptype = US"IPv6";
  else if (!Ustrncmp(p,CCS"UNKNOWN", 7))
    {
    iptype = US"Unknown";
    goto done;
    }
  else
    {
    DEBUG(D_receive) debug_printf("Invalid TCP type\n");
    goto proxyfail;
    }

  p += Ustrlen(iptype);
  if (!isspace(*(p++)))
    {
    DEBUG(D_receive) debug_printf("Missing space after TCP4/6 command\n");
    goto proxyfail;
    }
  /* Find the end of the arg */
  if ((sp = Ustrchr(p, ' ')) == NULL)
    {
    DEBUG(D_receive)
      debug_printf("Did not find proxied src %s\n", iptype);
    goto proxyfail;
    }
  *sp = '\0';
  if(!string_is_ip_address(p, NULL))
    {
    DEBUG(D_receive)
      debug_printf("Proxied src arg is not an %s address\n", iptype);
    goto proxyfail;
    }
  proxy_local_address = sender_host_address;
  sender_host_address = p;
  p = sp + 1;
  if ((sp = Ustrchr(p, ' ')) == NULL)
    {
    DEBUG(D_receive)
      debug_printf("Did not find proxy dest %s\n", iptype);
    goto proxyfail;
    }
  *sp = '\0';
  if(!string_is_ip_address(p, NULL))
    {
    DEBUG(D_receive)
      debug_printf("Proxy dest arg is not an %s address\n", iptype);
    goto proxyfail;
    }
  proxy_external_address = p;
  p = sp + 1;
  if ((sp = Ustrchr(p, ' ')) == NULL)
    {
    DEBUG(D_receive) debug_printf("Did not find proxied src port\n");
    goto proxyfail;
    }
  *sp = '\0';
  tmp_port = strtol(CCS p, &endc, 10);
  if (*endc || tmp_port == 0)
    {
    DEBUG(D_receive)
      debug_printf("Proxied src port '%s' not an integer\n", p);
    goto proxyfail;
    }
  proxy_local_port = sender_host_port;
  sender_host_port = tmp_port;
  p = sp + 1;
  if ((sp = Ustrchr(p, '\0')) == NULL)
    {
    DEBUG(D_receive) debug_printf("Did not find proxy dest port\n");
    goto proxyfail;
    }
  tmp_port = strtol(CCS p, &endc, 10);
  if (*endc || tmp_port == 0)
    {
    DEBUG(D_receive)
      debug_printf("Proxy dest port '%s' not an integer\n", p);
    goto proxyfail;
    }
  proxy_external_port = tmp_port;
  /* Already checked for /r /n above. Good V1 header received. */
  }
else
  {
  /* Wrong protocol */
  DEBUG(D_receive) debug_printf("Invalid proxy protocol version negotiation\n");
  (void) swallow_until_crlf(fd, (uschar*)&hdr, ret, sizeof(hdr)-ret);
  goto proxyfail;
  }

done:
  DEBUG(D_receive)
    debug_printf("Valid %s sender from Proxy Protocol header\n", iptype);
  yield = proxy_session;

/* Don't flush any potential buffer contents. Any input on proxyfail
should cause a synchronization failure */

proxyfail:
  restore_socket_timeout(fd, get_ok, &tvtmp, vslen);

bad:
  if (yield)
    {
    sender_host_name = NULL;
    (void) host_name_lookup();
    host_build_sender_fullhost();
    }
  else
    {
    f.proxy_session_failed = TRUE;
    DEBUG(D_receive)
      debug_printf("Failure to extract proxied host, only QUIT allowed\n");
    }

return;
}