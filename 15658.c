bgp_update_packet_eor (struct peer *peer, afi_t afi, safi_t safi)
{
  struct stream *s;
  struct stream *packet;

#ifdef DISABLE_BGP_ANNOUNCE
  return;
#endif /* DISABLE_BGP_ANNOUNCE */

  if (BGP_DEBUG (normal, NORMAL))
    zlog_debug ("send End-of-RIB for %s to %s", afi_safi_print (afi, safi), peer->host);

  s = stream_new (BGP_MAX_PACKET_SIZE);

  /* Make BGP update packet. */
  bgp_packet_set_marker (s, BGP_MSG_UPDATE);

  /* Unfeasible Routes Length */
  stream_putw (s, 0);

  if (afi == AFI_IP && safi == SAFI_UNICAST)
    {
      /* Total Path Attribute Length */
      stream_putw (s, 0);
    }
  else
    {
      /* Total Path Attribute Length */
      stream_putw (s, 6);
      stream_putc (s, BGP_ATTR_FLAG_OPTIONAL);
      stream_putc (s, BGP_ATTR_MP_UNREACH_NLRI);
      stream_putc (s, 3);
      stream_putw (s, afi);
      stream_putc (s, safi);
    }

  bgp_packet_set_size (s);
  packet = stream_dup (s);
  bgp_packet_add (peer, packet);
  stream_free (s);
  return packet;
}