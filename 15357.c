peer_port_vty (struct vty *vty, const char *ip_str, int afi, 
               const char *port_str)
{
  struct peer *peer;
  u_int16_t port;
  struct servent *sp;

  peer = peer_lookup_vty (vty, ip_str);
  if (! peer)
    return CMD_WARNING;

  if (! port_str)
    { 
      sp = getservbyname ("bgp", "tcp");
      port = (sp == NULL) ? BGP_PORT_DEFAULT : ntohs (sp->s_port);
    }
  else
    {
      VTY_GET_INTEGER("port", port, port_str);
    }

  peer_port_set (peer, port);

  return CMD_SUCCESS;
}