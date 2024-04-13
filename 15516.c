bgp_default_update_send (struct peer *peer, struct attr *attr,
			 afi_t afi, safi_t safi, struct peer *from)
{
  struct stream *s;
  struct stream *packet;
  struct prefix p;
  unsigned long pos;
  bgp_size_t total_attr_len;
  char attrstr[BUFSIZ];
  char buf[BUFSIZ];

#ifdef DISABLE_BGP_ANNOUNCE
  return;
#endif /* DISABLE_BGP_ANNOUNCE */

  if (afi == AFI_IP)
    str2prefix ("0.0.0.0/0", &p);
#ifdef HAVE_IPV6
  else 
    str2prefix ("::/0", &p);
#endif /* HAVE_IPV6 */

  /* Logging the attribute. */
  if (BGP_DEBUG (update, UPDATE_OUT))
    {
      bgp_dump_attr (peer, attr, attrstr, BUFSIZ);
      zlog (peer->log, LOG_DEBUG, "%s send UPDATE %s/%d %s",
	    peer->host, inet_ntop(p.family, &(p.u.prefix), buf, BUFSIZ),
	    p.prefixlen, attrstr);
    }

  s = stream_new (BGP_MAX_PACKET_SIZE);

  /* Make BGP update packet. */
  bgp_packet_set_marker (s, BGP_MSG_UPDATE);

  /* Unfeasible Routes Length. */
  stream_putw (s, 0);

  /* Make place for total attribute length.  */
  pos = stream_get_endp (s);
  stream_putw (s, 0);
  total_attr_len = bgp_packet_attribute (NULL, peer, s, attr, &p, afi, safi, from, NULL, NULL);

  /* Set Total Path Attribute Length. */
  stream_putw_at (s, pos, total_attr_len);

  /* NLRI set. */
  if (p.family == AF_INET && safi == SAFI_UNICAST)
    stream_put_prefix (s, &p);

  /* Set size. */
  bgp_packet_set_size (s);

  packet = stream_dup (s);
  stream_free (s);

  /* Dump packet if debug option is set. */
#ifdef DEBUG
  /* bgp_packet_dump (packet); */
#endif /* DEBUG */

  /* Add packet to the peer. */
  bgp_packet_add (peer, packet);

  BGP_WRITE_ON (peer->t_write, bgp_write, peer->fd);
}