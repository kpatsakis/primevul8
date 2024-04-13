bgp_update_receive (struct peer *peer, bgp_size_t size)
{
  int ret;
  u_char *end;
  struct stream *s;
  struct attr attr;
  bgp_size_t attribute_len;
  bgp_size_t update_len;
  bgp_size_t withdraw_len;
  struct bgp_nlri update;
  struct bgp_nlri withdraw;
  struct bgp_nlri mp_update;
  struct bgp_nlri mp_withdraw;
  char attrstr[BUFSIZ] = "";

  /* Status must be Established. */
  if (peer->status != Established) 
    {
      zlog_err ("%s [FSM] Update packet received under status %s",
		peer->host, LOOKUP (bgp_status_msg, peer->status));
      bgp_notify_send (peer, BGP_NOTIFY_FSM_ERR, 0);
      return -1;
    }

  /* Set initial values. */
  memset (&attr, 0, sizeof (struct attr));
  memset (&update, 0, sizeof (struct bgp_nlri));
  memset (&withdraw, 0, sizeof (struct bgp_nlri));
  memset (&mp_update, 0, sizeof (struct bgp_nlri));
  memset (&mp_withdraw, 0, sizeof (struct bgp_nlri));

  s = peer->ibuf;
  end = stream_pnt (s) + size;

  /* RFC1771 6.3 If the Unfeasible Routes Length or Total Attribute
     Length is too large (i.e., if Unfeasible Routes Length + Total
     Attribute Length + 23 exceeds the message Length), then the Error
     Subcode is set to Malformed Attribute List.  */
  if (stream_pnt (s) + 2 > end)
    {
      zlog_err ("%s [Error] Update packet error"
		" (packet length is short for unfeasible length)",
		peer->host);
      bgp_notify_send (peer, BGP_NOTIFY_UPDATE_ERR, 
		       BGP_NOTIFY_UPDATE_MAL_ATTR);
      return -1;
    }

  /* Unfeasible Route Length. */
  withdraw_len = stream_getw (s);

  /* Unfeasible Route Length check. */
  if (stream_pnt (s) + withdraw_len > end)
    {
      zlog_err ("%s [Error] Update packet error"
		" (packet unfeasible length overflow %d)",
		peer->host, withdraw_len);
      bgp_notify_send (peer, BGP_NOTIFY_UPDATE_ERR, 
		       BGP_NOTIFY_UPDATE_MAL_ATTR);
      return -1;
    }

  /* Unfeasible Route packet format check. */
  if (withdraw_len > 0)
    {
      ret = bgp_nlri_sanity_check (peer, AFI_IP, stream_pnt (s), withdraw_len);
      if (ret < 0)
	return -1;

      if (BGP_DEBUG (packet, PACKET_RECV))
	zlog_debug ("%s [Update:RECV] Unfeasible NLRI received", peer->host);

      withdraw.afi = AFI_IP;
      withdraw.safi = SAFI_UNICAST;
      withdraw.nlri = stream_pnt (s);
      withdraw.length = withdraw_len;
      stream_forward_getp (s, withdraw_len);
    }
  
  /* Attribute total length check. */
  if (stream_pnt (s) + 2 > end)
    {
      zlog_warn ("%s [Error] Packet Error"
		 " (update packet is short for attribute length)",
		 peer->host);
      bgp_notify_send (peer, BGP_NOTIFY_UPDATE_ERR, 
		       BGP_NOTIFY_UPDATE_MAL_ATTR);
      return -1;
    }

  /* Fetch attribute total length. */
  attribute_len = stream_getw (s);

  /* Attribute length check. */
  if (stream_pnt (s) + attribute_len > end)
    {
      zlog_warn ("%s [Error] Packet Error"
		 " (update packet attribute length overflow %d)",
		 peer->host, attribute_len);
      bgp_notify_send (peer, BGP_NOTIFY_UPDATE_ERR, 
		       BGP_NOTIFY_UPDATE_MAL_ATTR);
      return -1;
    }

  /* Parse attribute when it exists. */
  if (attribute_len)
    {
      ret = bgp_attr_parse (peer, &attr, attribute_len, 
			    &mp_update, &mp_withdraw);
      if (ret < 0)
	return -1;
    }

  /* Logging the attribute. */
  if (BGP_DEBUG (update, UPDATE_IN))
    {
      ret= bgp_dump_attr (peer, &attr, attrstr, BUFSIZ);

      if (ret)
	zlog (peer->log, LOG_DEBUG, "%s rcvd UPDATE w/ attr: %s",
	      peer->host, attrstr);
    }

  /* Network Layer Reachability Information. */
  update_len = end - stream_pnt (s);

  if (update_len)
    {
      /* Check NLRI packet format and prefix length. */
      ret = bgp_nlri_sanity_check (peer, AFI_IP, stream_pnt (s), update_len);
      if (ret < 0)
	return -1;

      /* Set NLRI portion to structure. */
      update.afi = AFI_IP;
      update.safi = SAFI_UNICAST;
      update.nlri = stream_pnt (s);
      update.length = update_len;
      stream_forward_getp (s, update_len);
    }

  /* NLRI is processed only when the peer is configured specific
     Address Family and Subsequent Address Family. */
  if (peer->afc[AFI_IP][SAFI_UNICAST])
    {
      if (withdraw.length)
	bgp_nlri_parse (peer, NULL, &withdraw);

      if (update.length)
	{
	  /* We check well-known attribute only for IPv4 unicast
	     update. */
	  ret = bgp_attr_check (peer, &attr);
	  if (ret < 0)
	    return -1;

	  bgp_nlri_parse (peer, &attr, &update);
	}

      if (mp_update.length
	  && mp_update.afi == AFI_IP 
	  && mp_update.safi == SAFI_UNICAST)
	bgp_nlri_parse (peer, &attr, &mp_update);

      if (mp_withdraw.length
	  && mp_withdraw.afi == AFI_IP 
	  && mp_withdraw.safi == SAFI_UNICAST)
	bgp_nlri_parse (peer, NULL, &mp_withdraw);

      if (! attribute_len && ! withdraw_len)
	{
	  /* End-of-RIB received */
	  SET_FLAG (peer->af_sflags[AFI_IP][SAFI_UNICAST],
		    PEER_STATUS_EOR_RECEIVED);

	  /* NSF delete stale route */
	  if (peer->nsf[AFI_IP][SAFI_UNICAST])
	    bgp_clear_stale_route (peer, AFI_IP, SAFI_UNICAST);

	  if (BGP_DEBUG (normal, NORMAL))
	    zlog (peer->log, LOG_DEBUG, "rcvd End-of-RIB for IPv4 Unicast from %s",
		  peer->host);
	}
    }
  if (peer->afc[AFI_IP][SAFI_MULTICAST])
    {
      if (mp_update.length
	  && mp_update.afi == AFI_IP 
	  && mp_update.safi == SAFI_MULTICAST)
	bgp_nlri_parse (peer, &attr, &mp_update);

      if (mp_withdraw.length
	  && mp_withdraw.afi == AFI_IP 
	  && mp_withdraw.safi == SAFI_MULTICAST)
	bgp_nlri_parse (peer, NULL, &mp_withdraw);

      if (! withdraw_len
	  && mp_withdraw.afi == AFI_IP
	  && mp_withdraw.safi == SAFI_MULTICAST
	  && mp_withdraw.length == 0)
	{
	  /* End-of-RIB received */
	  SET_FLAG (peer->af_sflags[AFI_IP][SAFI_MULTICAST],
		    PEER_STATUS_EOR_RECEIVED);

	  /* NSF delete stale route */
	  if (peer->nsf[AFI_IP][SAFI_MULTICAST])
	    bgp_clear_stale_route (peer, AFI_IP, SAFI_MULTICAST);

	  if (BGP_DEBUG (normal, NORMAL))
	    zlog (peer->log, LOG_DEBUG, "rcvd End-of-RIB for IPv4 Multicast from %s",
		  peer->host);
	}
    }
  if (peer->afc[AFI_IP6][SAFI_UNICAST])
    {
      if (mp_update.length 
	  && mp_update.afi == AFI_IP6 
	  && mp_update.safi == SAFI_UNICAST)
	bgp_nlri_parse (peer, &attr, &mp_update);

      if (mp_withdraw.length 
	  && mp_withdraw.afi == AFI_IP6 
	  && mp_withdraw.safi == SAFI_UNICAST)
	bgp_nlri_parse (peer, NULL, &mp_withdraw);

      if (! withdraw_len
	  && mp_withdraw.afi == AFI_IP6
	  && mp_withdraw.safi == SAFI_UNICAST
	  && mp_withdraw.length == 0)
	{
	  /* End-of-RIB received */
	  SET_FLAG (peer->af_sflags[AFI_IP6][SAFI_UNICAST], PEER_STATUS_EOR_RECEIVED);

	  /* NSF delete stale route */
	  if (peer->nsf[AFI_IP6][SAFI_UNICAST])
	    bgp_clear_stale_route (peer, AFI_IP6, SAFI_UNICAST);

	  if (BGP_DEBUG (normal, NORMAL))
	    zlog (peer->log, LOG_DEBUG, "rcvd End-of-RIB for IPv6 Unicast from %s",
		  peer->host);
	}
    }
  if (peer->afc[AFI_IP6][SAFI_MULTICAST])
    {
      if (mp_update.length 
	  && mp_update.afi == AFI_IP6 
	  && mp_update.safi == SAFI_MULTICAST)
	bgp_nlri_parse (peer, &attr, &mp_update);

      if (mp_withdraw.length 
	  && mp_withdraw.afi == AFI_IP6 
	  && mp_withdraw.safi == SAFI_MULTICAST)
	bgp_nlri_parse (peer, NULL, &mp_withdraw);

      if (! withdraw_len
	  && mp_withdraw.afi == AFI_IP6
	  && mp_withdraw.safi == SAFI_MULTICAST
	  && mp_withdraw.length == 0)
	{
	  /* End-of-RIB received */

	  /* NSF delete stale route */
	  if (peer->nsf[AFI_IP6][SAFI_MULTICAST])
	    bgp_clear_stale_route (peer, AFI_IP6, SAFI_MULTICAST);

	  if (BGP_DEBUG (update, UPDATE_IN))
	    zlog (peer->log, LOG_DEBUG, "rcvd End-of-RIB for IPv6 Multicast from %s",
		  peer->host);
	}
    }
  if (peer->afc[AFI_IP][SAFI_MPLS_VPN])
    {
      if (mp_update.length 
	  && mp_update.afi == AFI_IP 
	  && mp_update.safi == BGP_SAFI_VPNV4)
	bgp_nlri_parse_vpnv4 (peer, &attr, &mp_update);

      if (mp_withdraw.length 
	  && mp_withdraw.afi == AFI_IP 
	  && mp_withdraw.safi == BGP_SAFI_VPNV4)
	bgp_nlri_parse_vpnv4 (peer, NULL, &mp_withdraw);

      if (! withdraw_len
	  && mp_withdraw.afi == AFI_IP
	  && mp_withdraw.safi == BGP_SAFI_VPNV4
	  && mp_withdraw.length == 0)
	{
	  /* End-of-RIB received */

	  if (BGP_DEBUG (update, UPDATE_IN))
	    zlog (peer->log, LOG_DEBUG, "rcvd End-of-RIB for VPNv4 Unicast from %s",
		  peer->host);
	}
    }

  /* Everything is done.  We unintern temporary structures which
     interned in bgp_attr_parse(). */
  if (attr.aspath)
    aspath_unintern (attr.aspath);
  if (attr.community)
    community_unintern (attr.community);
  if (attr.extra)
    {
      if (attr.extra->ecommunity)
        ecommunity_unintern (attr.extra->ecommunity);
      if (attr.extra->cluster)
        cluster_unintern (attr.extra->cluster);
      if (attr.extra->transit)
        transit_unintern (attr.extra->transit);
      bgp_attr_extra_free (&attr);
    }

  /* If peering is stopped due to some reason, do not generate BGP
     event.  */
  if (peer->status != Established)
    return 0;

  /* Increment packet counter. */
  peer->update_in++;
  peer->update_time = time (NULL);

  /* Generate BGP event. */
  BGP_EVENT_ADD (peer, Receive_UPDATE_message);

  return 0;
}