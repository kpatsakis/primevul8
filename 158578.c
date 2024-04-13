smtp_port_for_connect(host_item * host, int port)
{
if (host->port != PORT_NONE)
  {
  HDEBUG(D_transport|D_acl|D_v)
    debug_printf_indent("Transport port=%d replaced by host-specific port=%d\n", port,
      host->port);
  port = host->port;
  }
else host->port = port;    /* Set the port actually used */
}