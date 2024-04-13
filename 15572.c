bgp_capability_send (struct peer *peer, afi_t afi, safi_t safi,
		     int capability_code, int action)
{
  struct stream *s;
  struct stream *packet;
  int length;

  /* Adjust safi code. */
  if (safi == SAFI_MPLS_VPN)
    safi = BGP_SAFI_VPNV4;

  s = stream_new (BGP_MAX_PACKET_SIZE);

  /* Make BGP update packet. */
  bgp_packet_set_marker (s, BGP_MSG_CAPABILITY);

  /* Encode MP_EXT capability. */
  if (capability_code == CAPABILITY_CODE_MP)
    {
      stream_putc (s, action);
      stream_putc (s, CAPABILITY_CODE_MP);
      stream_putc (s, CAPABILITY_CODE_MP_LEN);
      stream_putw (s, afi);
      stream_putc (s, 0);
      stream_putc (s, safi);

      if (BGP_DEBUG (normal, NORMAL))
        zlog_debug ("%s sending CAPABILITY has %s MP_EXT CAP for afi/safi: %d/%d",
		   peer->host, action == CAPABILITY_ACTION_SET ?
		   "Advertising" : "Removing", afi, safi);
    }

  /* Set packet size. */
  length = bgp_packet_set_size (s);

  /* Make real packet. */
  packet = stream_dup (s);
  stream_free (s);

  /* Add packet to the peer. */
  bgp_packet_add (peer, packet);

  if (BGP_DEBUG (normal, NORMAL))
    zlog_debug ("%s send message type %d, length (incl. header) %d",
	       peer->host, BGP_MSG_CAPABILITY, length);

  BGP_WRITE_ON (peer->t_write, bgp_write, peer->fd);
}