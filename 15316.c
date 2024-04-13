bgp_route_refresh_receive (struct peer *peer, bgp_size_t size)
{
  afi_t afi;
  safi_t safi;
  u_char reserved;
  struct stream *s;

  /* If peer does not have the capability, send notification. */
  if (! CHECK_FLAG (peer->cap, PEER_CAP_REFRESH_ADV))
    {
      plog_err (peer->log, "%s [Error] BGP route refresh is not enabled",
		peer->host);
      bgp_notify_send (peer,
		       BGP_NOTIFY_HEADER_ERR,
		       BGP_NOTIFY_HEADER_BAD_MESTYPE);
      return;
    }

  /* Status must be Established. */
  if (peer->status != Established) 
    {
      plog_err (peer->log,
		"%s [Error] Route refresh packet received under status %s",
		peer->host, LOOKUP (bgp_status_msg, peer->status));
      bgp_notify_send (peer, BGP_NOTIFY_FSM_ERR, 0);
      return;
    }

  s = peer->ibuf;
  
  /* Parse packet. */
  afi = stream_getw (s);
  reserved = stream_getc (s);
  safi = stream_getc (s);

  if (BGP_DEBUG (normal, NORMAL))
    zlog_debug ("%s rcvd REFRESH_REQ for afi/safi: %d/%d",
	       peer->host, afi, safi);

  /* Check AFI and SAFI. */
  if ((afi != AFI_IP && afi != AFI_IP6)
      || (safi != SAFI_UNICAST && safi != SAFI_MULTICAST
	  && safi != BGP_SAFI_VPNV4))
    {
      if (BGP_DEBUG (normal, NORMAL))
	{
	  zlog_debug ("%s REFRESH_REQ for unrecognized afi/safi: %d/%d - ignored",
		     peer->host, afi, safi);
	}
      return;
    }

  /* Adjust safi code. */
  if (safi == BGP_SAFI_VPNV4)
    safi = SAFI_MPLS_VPN;

  if (size != BGP_MSG_ROUTE_REFRESH_MIN_SIZE - BGP_HEADER_SIZE)
    {
      u_char *end;
      u_char when_to_refresh;
      u_char orf_type;
      u_int16_t orf_len;

      if (size - (BGP_MSG_ROUTE_REFRESH_MIN_SIZE - BGP_HEADER_SIZE) < 5)
        {
          zlog_info ("%s ORF route refresh length error", peer->host);
          bgp_notify_send (peer, BGP_NOTIFY_CEASE, 0);
          return;
        }

      when_to_refresh = stream_getc (s);
      end = stream_pnt (s) + (size - 5);

      while (stream_pnt (s) < end)
	{
	  orf_type = stream_getc (s); 
	  orf_len = stream_getw (s);

	  if (orf_type == ORF_TYPE_PREFIX
	      || orf_type == ORF_TYPE_PREFIX_OLD)
	    {
	      u_char *p_pnt = stream_pnt (s);
	      u_char *p_end = stream_pnt (s) + orf_len;
	      struct orf_prefix orfp;
	      u_char common = 0;
	      u_int32_t seq;
	      int psize;
	      char name[BUFSIZ];
	      char buf[BUFSIZ];
	      int ret;

	      if (BGP_DEBUG (normal, NORMAL))
		{
		  zlog_debug ("%s rcvd Prefixlist ORF(%d) length %d",
			     peer->host, orf_type, orf_len);
		}

	      /* ORF prefix-list name */
	      sprintf (name, "%s.%d.%d", peer->host, afi, safi);

	      while (p_pnt < p_end)
		{
		  memset (&orfp, 0, sizeof (struct orf_prefix));
		  common = *p_pnt++;
		  if (common & ORF_COMMON_PART_REMOVE_ALL)
		    {
		      if (BGP_DEBUG (normal, NORMAL))
			zlog_debug ("%s rcvd Remove-All pfxlist ORF request", peer->host);
		      prefix_bgp_orf_remove_all (name);
		      break;
		    }
		  memcpy (&seq, p_pnt, sizeof (u_int32_t));
		  p_pnt += sizeof (u_int32_t);
		  orfp.seq = ntohl (seq);
		  orfp.ge = *p_pnt++;
		  orfp.le = *p_pnt++;
		  orfp.p.prefixlen = *p_pnt++;
		  orfp.p.family = afi2family (afi);
		  psize = PSIZE (orfp.p.prefixlen);
		  memcpy (&orfp.p.u.prefix, p_pnt, psize);
		  p_pnt += psize;

		  if (BGP_DEBUG (normal, NORMAL))
		    zlog_debug ("%s rcvd %s %s seq %u %s/%d ge %d le %d",
			       peer->host,
			       (common & ORF_COMMON_PART_REMOVE ? "Remove" : "Add"), 
			       (common & ORF_COMMON_PART_DENY ? "deny" : "permit"),
			       orfp.seq, 
			       inet_ntop (orfp.p.family, &orfp.p.u.prefix, buf, BUFSIZ),
			       orfp.p.prefixlen, orfp.ge, orfp.le);

		  ret = prefix_bgp_orf_set (name, afi, &orfp,
				 (common & ORF_COMMON_PART_DENY ? 0 : 1 ),
				 (common & ORF_COMMON_PART_REMOVE ? 0 : 1));

		  if (ret != CMD_SUCCESS)
		    {
		      if (BGP_DEBUG (normal, NORMAL))
			zlog_debug ("%s Received misformatted prefixlist ORF. Remove All pfxlist", peer->host);
		      prefix_bgp_orf_remove_all (name);
		      break;
		    }
		}
	      peer->orf_plist[afi][safi] =
			 prefix_list_lookup (AFI_ORF_PREFIX, name);
	    }
	  stream_forward_getp (s, orf_len);
	}
      if (BGP_DEBUG (normal, NORMAL))
	zlog_debug ("%s rcvd Refresh %s ORF request", peer->host,
		   when_to_refresh == REFRESH_DEFER ? "Defer" : "Immediate");
      if (when_to_refresh == REFRESH_DEFER)
	return;
    }

  /* First update is deferred until ORF or ROUTE-REFRESH is received */
  if (CHECK_FLAG (peer->af_sflags[afi][safi], PEER_STATUS_ORF_WAIT_REFRESH))
    UNSET_FLAG (peer->af_sflags[afi][safi], PEER_STATUS_ORF_WAIT_REFRESH);

  /* Perform route refreshment to the peer */
  bgp_announce_route (peer, afi, safi);
}