bgp_open_option_parse (struct peer *peer, u_char length)
{
  int ret;
  u_char *error;
  u_char error_data[BGP_MAX_PACKET_SIZE];
  struct stream *s = BGP_INPUT(peer);
  size_t end = stream_get_getp (s) + length;
  int mp_capability;

  ret = 0;
  error = error_data;

  if (BGP_DEBUG (normal, NORMAL))
    zlog_debug ("%s rcv OPEN w/ OPTION parameter len: %u",
	       peer->host, length);
  
  while (stream_get_getp(s) < end)
    {
      u_char opt_type;
      u_char opt_length;
      
      /* Must have at least an OPEN option header */
      if (STREAM_READABLE(s) < 2)
	{
	  zlog_info ("%s Option length error", peer->host);
	  bgp_notify_send (peer, BGP_NOTIFY_CEASE, 0);
	  return -1;
	}

      /* Fetch option type and length. */
      opt_type = stream_getc (s);
      opt_length = stream_getc (s);
      
      /* Option length check. */
      if (STREAM_READABLE (s) < opt_length)
	{
	  zlog_info ("%s Option length error", peer->host);
	  bgp_notify_send (peer, BGP_NOTIFY_CEASE, 0);
	  return -1;
	}

      if (BGP_DEBUG (normal, NORMAL))
	zlog_debug ("%s rcvd OPEN w/ optional parameter type %u (%s) len %u",
		   peer->host, opt_type,
		   opt_type == BGP_OPEN_OPT_AUTH ? "Authentication" :
		   opt_type == BGP_OPEN_OPT_CAP ? "Capability" : "Unknown",
		   opt_length);
  
      switch (opt_type)
	{
	case BGP_OPEN_OPT_AUTH:
	  ret = bgp_auth_parse (peer, opt_length);
	  break;
	case BGP_OPEN_OPT_CAP:
	  ret = bgp_capability_parse (peer, opt_length, &mp_capability, &error);
	  break;
	default:
	  bgp_notify_send (peer, 
			   BGP_NOTIFY_OPEN_ERR, 
			   BGP_NOTIFY_OPEN_UNSUP_PARAM); 
	  ret = -1;
	  break;
	}

      /* Parse error.  To accumulate all unsupported capability codes,
         bgp_capability_parse does not return -1 when encounter
         unsupported capability code.  To detect that, please check
         error and erro_data pointer, like below.  */
      if (ret < 0)
	return -1;
    }

  /* All OPEN option is parsed.  Check capability when strict compare
     flag is enabled.*/
  if (CHECK_FLAG (peer->flags, PEER_FLAG_STRICT_CAP_MATCH))
    {
      /* If Unsupported Capability exists. */
      if (error != error_data)
	{
	  bgp_notify_send_with_data (peer, 
				     BGP_NOTIFY_OPEN_ERR, 
				     BGP_NOTIFY_OPEN_UNSUP_CAPBL, 
				     error_data, error - error_data);
	  return -1;
	}

      /* Check local capability does not negotiated with remote
         peer. */
      if (! strict_capability_same (peer))
	{
	  bgp_notify_send (peer, 
			   BGP_NOTIFY_OPEN_ERR, 
			   BGP_NOTIFY_OPEN_UNSUP_CAPBL);
	  return -1;
	}
    }

  if (!mp_capability)
    {
      peer->afc_nego[AFI_IP][SAFI_UNICAST] = peer->afc[AFI_IP][SAFI_UNICAST];
    }
  /* Check there are no common AFI/SAFIs and send Unsupported Capability
     error. */
  if (CHECK_FLAG (peer->flags, PEER_FLAG_OVERRIDE_CAPABILITY))
    {
      peer->afc_nego[AFI_IP][SAFI_UNICAST] = peer->afc[AFI_IP][SAFI_UNICAST];
      peer->afc_nego[AFI_IP][SAFI_MULTICAST] = peer->afc[AFI_IP][SAFI_MULTICAST];
      peer->afc_nego[AFI_IP][SAFI_MPLS_VPN] = peer->afc[AFI_IP][SAFI_MPLS_VPN];
      peer->afc_nego[AFI_IP6][SAFI_UNICAST] = peer->afc[AFI_IP6][SAFI_UNICAST];
      peer->afc_nego[AFI_IP6][SAFI_MULTICAST] = peer->afc[AFI_IP6][SAFI_MULTICAST];
    }
  else
    {
      if (! peer->afc_nego[AFI_IP][SAFI_UNICAST] 
	  && ! peer->afc_nego[AFI_IP][SAFI_MULTICAST]
	  && ! peer->afc_nego[AFI_IP][SAFI_MPLS_VPN]
	  && ! peer->afc_nego[AFI_IP6][SAFI_UNICAST]
	  && ! peer->afc_nego[AFI_IP6][SAFI_MULTICAST])
	{
	  plog_err (peer->log, "%s [Error] Configured AFI/SAFIs do not "
		    "overlap with received MP capabilities",
		    peer->host);

	  if (error != error_data)

	    bgp_notify_send_with_data (peer, 
				       BGP_NOTIFY_OPEN_ERR, 
				       BGP_NOTIFY_OPEN_UNSUP_CAPBL, 
				       error_data, error - error_data);
	  else
	    bgp_notify_send (peer, 
			     BGP_NOTIFY_OPEN_ERR, 
			     BGP_NOTIFY_OPEN_UNSUP_CAPBL);
	  return -1;
	}
    }
  return 0;
}