bgp_route_refresh_send (struct peer *peer, afi_t afi, safi_t safi,
			u_char orf_type, u_char when_to_refresh, int remove)
{
  struct stream *s;
  struct stream *packet;
  int length;
  struct bgp_filter *filter;
  int orf_refresh = 0;

#ifdef DISABLE_BGP_ANNOUNCE
  return;
#endif /* DISABLE_BGP_ANNOUNCE */

  filter = &peer->filter[afi][safi];

  /* Adjust safi code. */
  if (safi == SAFI_MPLS_VPN)
    safi = BGP_SAFI_VPNV4;
  
  s = stream_new (BGP_MAX_PACKET_SIZE);

  /* Make BGP update packet. */
  if (CHECK_FLAG (peer->cap, PEER_CAP_REFRESH_NEW_RCV))
    bgp_packet_set_marker (s, BGP_MSG_ROUTE_REFRESH_NEW);
  else
    bgp_packet_set_marker (s, BGP_MSG_ROUTE_REFRESH_OLD);

  /* Encode Route Refresh message. */
  stream_putw (s, afi);
  stream_putc (s, 0);
  stream_putc (s, safi);
 
  if (orf_type == ORF_TYPE_PREFIX
      || orf_type == ORF_TYPE_PREFIX_OLD)
    if (remove || filter->plist[FILTER_IN].plist)
      {
	u_int16_t orf_len;
	unsigned long orfp;

	orf_refresh = 1; 
	stream_putc (s, when_to_refresh);
	stream_putc (s, orf_type);
	orfp = stream_get_endp (s);
	stream_putw (s, 0);

	if (remove)
	  {
	    UNSET_FLAG (peer->af_sflags[afi][safi], PEER_STATUS_ORF_PREFIX_SEND);
	    stream_putc (s, ORF_COMMON_PART_REMOVE_ALL);
	    if (BGP_DEBUG (normal, NORMAL))
	      zlog_debug ("%s sending REFRESH_REQ to remove ORF(%d) (%s) for afi/safi: %d/%d", 
			 peer->host, orf_type,
			 (when_to_refresh == REFRESH_DEFER ? "defer" : "immediate"),
			 afi, safi);
	  }
	else
	  {
	    SET_FLAG (peer->af_sflags[afi][safi], PEER_STATUS_ORF_PREFIX_SEND);
	    prefix_bgp_orf_entry (s, filter->plist[FILTER_IN].plist,
				  ORF_COMMON_PART_ADD, ORF_COMMON_PART_PERMIT,
				  ORF_COMMON_PART_DENY);
	    if (BGP_DEBUG (normal, NORMAL))
	      zlog_debug ("%s sending REFRESH_REQ with pfxlist ORF(%d) (%s) for afi/safi: %d/%d", 
			 peer->host, orf_type,
			 (when_to_refresh == REFRESH_DEFER ? "defer" : "immediate"),
			 afi, safi);
	  }

	/* Total ORF Entry Len. */
	orf_len = stream_get_endp (s) - orfp - 2;
	stream_putw_at (s, orfp, orf_len);
      }

  /* Set packet size. */
  length = bgp_packet_set_size (s);

  if (BGP_DEBUG (normal, NORMAL))
    {
      if (! orf_refresh)
	zlog_debug ("%s sending REFRESH_REQ for afi/safi: %d/%d", 
		   peer->host, afi, safi);
      zlog_debug ("%s send message type %d, length (incl. header) %d",
		 peer->host, CHECK_FLAG (peer->cap, PEER_CAP_REFRESH_NEW_RCV) ?
		 BGP_MSG_ROUTE_REFRESH_NEW : BGP_MSG_ROUTE_REFRESH_OLD, length);
    }

  /* Make real packet. */
  packet = stream_dup (s);
  stream_free (s);

  /* Add packet to the peer. */
  bgp_packet_add (peer, packet);

  BGP_WRITE_ON (peer->t_write, bgp_write, peer->fd);
}