bgp_default_withdraw_send (struct peer *peer, afi_t afi, safi_t safi)
{
  struct stream *s;
  struct stream *packet;
  struct prefix p;
  unsigned long pos;
  unsigned long cp;
  bgp_size_t unfeasible_len;
  bgp_size_t total_attr_len;
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

  total_attr_len = 0;
  pos = 0;

  if (BGP_DEBUG (update, UPDATE_OUT))
    zlog (peer->log, LOG_DEBUG, "%s send UPDATE %s/%d -- unreachable",
          peer->host, inet_ntop(p.family, &(p.u.prefix), buf, BUFSIZ),
          p.prefixlen);

  s = stream_new (BGP_MAX_PACKET_SIZE);

  /* Make BGP update packet. */
  bgp_packet_set_marker (s, BGP_MSG_UPDATE);

  /* Unfeasible Routes Length. */;
  cp = stream_get_endp (s);
  stream_putw (s, 0);

  /* Withdrawn Routes. */
  if (p.family == AF_INET && safi == SAFI_UNICAST)
    {
      stream_put_prefix (s, &p);

      unfeasible_len = stream_get_endp (s) - cp - 2;

      /* Set unfeasible len.  */
      stream_putw_at (s, cp, unfeasible_len);

      /* Set total path attribute length. */
      stream_putw (s, 0);
    }
  else
    {
      pos = stream_get_endp (s);
      stream_putw (s, 0);
      total_attr_len = bgp_packet_withdraw (peer, s, &p, afi, safi, NULL, NULL);

      /* Set total path attribute length. */
      stream_putw_at (s, pos, total_attr_len);
    }

  bgp_packet_set_size (s);

  packet = stream_dup (s);
  stream_free (s);

  /* Add packet to the peer. */
  bgp_packet_add (peer, packet);

  BGP_WRITE_ON (peer->t_write, bgp_write, peer->fd);
}