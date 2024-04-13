bgp_clear (struct vty *vty, struct bgp *bgp,  afi_t afi, safi_t safi,
           enum clear_sort sort,enum bgp_clear_type stype, const char *arg)
{
  int ret;
  struct peer *peer;
  struct listnode *node, *nnode;

  /* Clear all neighbors. */
  if (sort == clear_all)
    {
      for (ALL_LIST_ELEMENTS (bgp->peer, node, nnode, peer))
	{
	  if (stype == BGP_CLEAR_SOFT_NONE)
	    ret = peer_clear (peer);
	  else
	    ret = peer_clear_soft (peer, afi, safi, stype);

	  if (ret < 0)
	    bgp_clear_vty_error (vty, peer, afi, safi, ret);
	}
      return 0;
    }

  /* Clear specified neighbors. */
  if (sort == clear_peer)
    {
      union sockunion su;
      int ret;

      /* Make sockunion for lookup. */
      ret = str2sockunion (arg, &su);
      if (ret < 0)
	{
	  vty_out (vty, "Malformed address: %s%s", arg, VTY_NEWLINE);
	  return -1;
	}
      peer = peer_lookup (bgp, &su);
      if (! peer)
	{
	  vty_out (vty, "%%BGP: Unknown neighbor - \"%s\"%s", arg, VTY_NEWLINE);
	  return -1;
	}

      if (stype == BGP_CLEAR_SOFT_NONE)
	ret = peer_clear (peer);
      else
	ret = peer_clear_soft (peer, afi, safi, stype);

      if (ret < 0)
	bgp_clear_vty_error (vty, peer, afi, safi, ret);

      return 0;
    }

  /* Clear all peer-group members. */
  if (sort == clear_group)
    {
      struct peer_group *group;

      group = peer_group_lookup (bgp, arg);
      if (! group)
	{
	  vty_out (vty, "%%BGP: No such peer-group %s%s", arg, VTY_NEWLINE);
	  return -1; 
	}

      for (ALL_LIST_ELEMENTS (group->peer, node, nnode, peer))
	{
	  if (stype == BGP_CLEAR_SOFT_NONE)
	    {
	      ret = peer_clear (peer);
	      continue;
	    }

	  if (! peer->af_group[afi][safi])
	    continue;

	  ret = peer_clear_soft (peer, afi, safi, stype);

	  if (ret < 0)
	    bgp_clear_vty_error (vty, peer, afi, safi, ret);
	}
      return 0;
    }

  if (sort == clear_external)
    {
      for (ALL_LIST_ELEMENTS (bgp->peer, node, nnode, peer))
	{
	  if (peer_sort (peer) == BGP_PEER_IBGP) 
	    continue;

	  if (stype == BGP_CLEAR_SOFT_NONE)
	    ret = peer_clear (peer);
	  else
	    ret = peer_clear_soft (peer, afi, safi, stype);

	  if (ret < 0)
	    bgp_clear_vty_error (vty, peer, afi, safi, ret);
	}
      return 0;
    }

  if (sort == clear_as)
    {
      as_t as;
      unsigned long as_ul;
      char *endptr = NULL;
      int find = 0;

      as_ul = strtoul(arg, &endptr, 10);

      if ((as_ul == ULONG_MAX) || (*endptr != '\0') || (as_ul > USHRT_MAX))
	{
	  vty_out (vty, "Invalid AS number%s", VTY_NEWLINE); 
	  return -1;
	}
      as = (as_t) as_ul;

      for (ALL_LIST_ELEMENTS (bgp->peer, node, nnode, peer))
	{
	  if (peer->as != as) 
	    continue;

	  find = 1;
	  if (stype == BGP_CLEAR_SOFT_NONE)
	    ret = peer_clear (peer);
	  else
	    ret = peer_clear_soft (peer, afi, safi, stype);

	  if (ret < 0)
	    bgp_clear_vty_error (vty, peer, afi, safi, ret);
	}
      if (! find)
	vty_out (vty, "%%BGP: No peer is configured with AS %s%s", arg,
		 VTY_NEWLINE);
      return 0;
    }

  return 0;
}