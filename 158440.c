smtp_sock_connect(host_item * host, int host_af, int port, uschar * interface,
  transport_instance * tb, int timeout, const blob * early_data)
{
smtp_transport_options_block * ob =
  (smtp_transport_options_block *)tb->options_block;
const uschar * dscp = ob->dscp;
int dscp_value;
int dscp_level;
int dscp_option;
int sock;
int save_errno = 0;
const blob * fastopen_blob = NULL;


#ifndef DISABLE_EVENT
deliver_host_address = host->address;
deliver_host_port = port;
if (event_raise(tb->event_action, US"tcp:connect", NULL)) return -1;
#endif

if ((sock = ip_socket(SOCK_STREAM, host_af)) < 0) return -1;

/* Set TCP_NODELAY; Exim does its own buffering. */

if (setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, US &on, sizeof(on)))
  HDEBUG(D_transport|D_acl|D_v)
    debug_printf_indent("failed to set NODELAY: %s ", strerror(errno));

/* Set DSCP value, if we can. For now, if we fail to set the value, we don't
bomb out, just log it and continue in default traffic class. */

if (dscp && dscp_lookup(dscp, host_af, &dscp_level, &dscp_option, &dscp_value))
  {
  HDEBUG(D_transport|D_acl|D_v)
    debug_printf_indent("DSCP \"%s\"=%x ", dscp, dscp_value);
  if (setsockopt(sock, dscp_level, dscp_option, &dscp_value, sizeof(dscp_value)) < 0)
    HDEBUG(D_transport|D_acl|D_v)
      debug_printf_indent("failed to set DSCP: %s ", strerror(errno));
  /* If the kernel supports IPv4 and IPv6 on an IPv6 socket, we need to set the
  option for both; ignore failures here */
  if (host_af == AF_INET6 &&
      dscp_lookup(dscp, AF_INET, &dscp_level, &dscp_option, &dscp_value))
    (void) setsockopt(sock, dscp_level, dscp_option, &dscp_value, sizeof(dscp_value));
  }

/* Bind to a specific interface if requested. Caller must ensure the interface
is the same type (IPv4 or IPv6) as the outgoing address. */

if (interface && ip_bind(sock, host_af, interface, 0) < 0)
  {
  save_errno = errno;
  HDEBUG(D_transport|D_acl|D_v)
    debug_printf_indent("unable to bind outgoing SMTP call to %s: %s", interface,
    strerror(errno));
  }

/* Connect to the remote host, and add keepalive to the socket before returning
it, if requested.  If the build supports TFO, request it - and if the caller
requested some early-data then include that in the TFO request.  If there is
early-data but no TFO support, send it after connecting. */

else
  {
#ifdef TCP_FASTOPEN
  if (verify_check_given_host(CUSS &ob->hosts_try_fastopen, host) == OK)
    fastopen_blob = early_data ? early_data : &tcp_fastopen_nodata;
#endif

  if (ip_connect(sock, host_af, host->address, port, timeout, fastopen_blob) < 0)
    save_errno = errno;
  else if (early_data && !fastopen_blob && early_data->data && early_data->len)
    {
    HDEBUG(D_transport|D_acl|D_v)
      debug_printf("sending %ld nonTFO early-data\n", (long)early_data->len);

#ifdef TCP_QUICKACK
    (void) setsockopt(sock, IPPROTO_TCP, TCP_QUICKACK, US &off, sizeof(off));
#endif
    if (send(sock, early_data->data, early_data->len, 0) < 0)
      save_errno = errno;
    }
  }

/* Either bind() or connect() failed */

if (save_errno != 0)
  {
  HDEBUG(D_transport|D_acl|D_v)
    {
    debug_printf_indent("failed: %s", CUstrerror(save_errno));
    if (save_errno == ETIMEDOUT)
      debug_printf(" (timeout=%s)", readconf_printtime(timeout));
    debug_printf("\n");
    }
  (void)close(sock);
  errno = save_errno;
  return -1;
  }

/* Both bind() and connect() succeeded, and any early-data */

else
  {
  union sockaddr_46 interface_sock;
  EXIM_SOCKLEN_T size = sizeof(interface_sock);

  HDEBUG(D_transport|D_acl|D_v) debug_printf_indent("connected\n");
  if (getsockname(sock, (struct sockaddr *)(&interface_sock), &size) == 0)
    sending_ip_address = host_ntoa(-1, &interface_sock, NULL, &sending_port);
  else
    {
    log_write(0, LOG_MAIN | ((errno == ECONNRESET)? 0 : LOG_PANIC),
      "getsockname() failed: %s", strerror(errno));
    close(sock);
    return -1;
    }

  if (ob->keepalive) ip_keepalive(sock, host->address, TRUE);
#ifdef TCP_FASTOPEN
  tfo_out_check(sock);
#endif
  return sock;
  }
}