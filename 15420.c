bgp_config_write_redistribute (struct vty *vty, struct bgp *bgp, afi_t afi,
			       safi_t safi, int *write)
{
  int i;

  /* Unicast redistribution only.  */
  if (safi != SAFI_UNICAST)
    return 0;

  for (i = 0; i < ZEBRA_ROUTE_MAX; i++)
    {
      /* Redistribute BGP does not make sense.  */
      if (bgp->redist[afi][i] && i != ZEBRA_ROUTE_BGP)
	{
	  /* Display "address-family" when it is not yet diplayed.  */
	  bgp_config_write_family_header (vty, afi, safi, write);

	  /* "redistribute" configuration.  */
	  vty_out (vty, " redistribute %s", zebra_route_string(i));

	  if (bgp->redist_metric_flag[afi][i])
	    vty_out (vty, " metric %d", bgp->redist_metric[afi][i]);

	  if (bgp->rmap[afi][i].name)
	    vty_out (vty, " route-map %s", bgp->rmap[afi][i].name);

	  vty_out (vty, "%s", VTY_NEWLINE);
	}
    }
  return *write;
}