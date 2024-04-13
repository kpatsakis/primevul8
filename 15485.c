bgp_open_send (struct peer *peer)
{
  struct stream *s;
  int length;
  u_int16_t send_holdtime;
  as_t local_as;

  if (CHECK_FLAG (peer->config, PEER_CONFIG_TIMER))
    send_holdtime = peer->holdtime;
  else
    send_holdtime = peer->bgp->default_holdtime;

  /* local-as Change */
  if (peer->change_local_as)
    local_as = peer->change_local_as; 
  else
    local_as = peer->local_as; 

  s = stream_new (BGP_MAX_PACKET_SIZE);

  /* Make open packet. */
  bgp_packet_set_marker (s, BGP_MSG_OPEN);

  /* Set open packet values. */
  stream_putc (s, BGP_VERSION_4);        /* BGP version */
  stream_putw (s, local_as);		 /* My Autonomous System*/
  stream_putw (s, send_holdtime);     	 /* Hold Time */
  stream_put_in_addr (s, &peer->local_id); /* BGP Identifier */

  /* Set capability code. */
  bgp_open_capability (s, peer);

  /* Set BGP packet length. */
  length = bgp_packet_set_size (s);

  if (BGP_DEBUG (normal, NORMAL))
    zlog_debug ("%s sending OPEN, version %d, my as %d, holdtime %d, id %s", 
	       peer->host, BGP_VERSION_4, local_as,
	       send_holdtime, inet_ntoa (peer->local_id));

  if (BGP_DEBUG (normal, NORMAL))
    zlog_debug ("%s send message type %d, length (incl. header) %d",
	       peer->host, BGP_MSG_OPEN, length);

  /* Dump packet if debug option is set. */
  /* bgp_packet_dump (s); */

  /* Add packet to the peer. */
  bgp_packet_add (peer, s);

  BGP_WRITE_ON (peer->t_write, bgp_write, peer->fd);
}