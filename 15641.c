bgp_show_peer_afi (struct vty *vty, struct peer *p, afi_t afi, safi_t safi)
{
  struct bgp_filter *filter;
  char orf_pfx_name[BUFSIZ];
  int orf_pfx_count;

  filter = &p->filter[afi][safi];

  vty_out (vty, " For address family: %s%s", afi_safi_print (afi, safi),
	   VTY_NEWLINE);

  if (p->af_group[afi][safi])
    vty_out (vty, "  %s peer-group member%s", p->group->name, VTY_NEWLINE);

  if (CHECK_FLAG (p->af_cap[afi][safi], PEER_CAP_ORF_PREFIX_SM_ADV)
      || CHECK_FLAG (p->af_cap[afi][safi], PEER_CAP_ORF_PREFIX_SM_RCV)
      || CHECK_FLAG (p->af_cap[afi][safi], PEER_CAP_ORF_PREFIX_SM_OLD_RCV)
      || CHECK_FLAG (p->af_cap[afi][safi], PEER_CAP_ORF_PREFIX_RM_ADV)
      || CHECK_FLAG (p->af_cap[afi][safi], PEER_CAP_ORF_PREFIX_RM_RCV)
      || CHECK_FLAG (p->af_cap[afi][safi], PEER_CAP_ORF_PREFIX_RM_OLD_RCV))
    vty_out (vty, "  AF-dependant capabilities:%s", VTY_NEWLINE);

  if (CHECK_FLAG (p->af_cap[afi][safi], PEER_CAP_ORF_PREFIX_SM_ADV)
      || CHECK_FLAG (p->af_cap[afi][safi], PEER_CAP_ORF_PREFIX_SM_RCV)
      || CHECK_FLAG (p->af_cap[afi][safi], PEER_CAP_ORF_PREFIX_RM_ADV)
      || CHECK_FLAG (p->af_cap[afi][safi], PEER_CAP_ORF_PREFIX_RM_RCV))
    {
      vty_out (vty, "    Outbound Route Filter (ORF) type (%d) Prefix-list:%s",
	       ORF_TYPE_PREFIX, VTY_NEWLINE);
      bgp_show_peer_afi_orf_cap (vty, p, afi, safi,
				 PEER_CAP_ORF_PREFIX_SM_ADV,
				 PEER_CAP_ORF_PREFIX_RM_ADV,
				 PEER_CAP_ORF_PREFIX_SM_RCV,
				 PEER_CAP_ORF_PREFIX_RM_RCV);
    }
  if (CHECK_FLAG (p->af_cap[afi][safi], PEER_CAP_ORF_PREFIX_SM_ADV)
      || CHECK_FLAG (p->af_cap[afi][safi], PEER_CAP_ORF_PREFIX_SM_OLD_RCV)
      || CHECK_FLAG (p->af_cap[afi][safi], PEER_CAP_ORF_PREFIX_RM_ADV)
      || CHECK_FLAG (p->af_cap[afi][safi], PEER_CAP_ORF_PREFIX_RM_OLD_RCV))
    {
      vty_out (vty, "    Outbound Route Filter (ORF) type (%d) Prefix-list:%s",
	       ORF_TYPE_PREFIX_OLD, VTY_NEWLINE);
      bgp_show_peer_afi_orf_cap (vty, p, afi, safi,
				 PEER_CAP_ORF_PREFIX_SM_ADV,
				 PEER_CAP_ORF_PREFIX_RM_ADV,
				 PEER_CAP_ORF_PREFIX_SM_OLD_RCV,
				 PEER_CAP_ORF_PREFIX_RM_OLD_RCV);
    }

  sprintf (orf_pfx_name, "%s.%d.%d", p->host, afi, safi);
  orf_pfx_count =  prefix_bgp_show_prefix_list (NULL, afi, orf_pfx_name);

  if (CHECK_FLAG (p->af_sflags[afi][safi], PEER_STATUS_ORF_PREFIX_SEND)
      || orf_pfx_count)
    {
      vty_out (vty, "  Outbound Route Filter (ORF):");
      if (CHECK_FLAG (p->af_sflags[afi][safi], PEER_STATUS_ORF_PREFIX_SEND))
	  vty_out (vty, " sent;");
      if (orf_pfx_count)
	vty_out (vty, " received (%d entries)", orf_pfx_count);
      vty_out (vty, "%s", VTY_NEWLINE);
    }
  if (CHECK_FLAG (p->af_sflags[afi][safi], PEER_STATUS_ORF_WAIT_REFRESH))
      vty_out (vty, "  First update is deferred until ORF or ROUTE-REFRESH is received%s", VTY_NEWLINE);

  if (CHECK_FLAG (p->af_flags[afi][safi], PEER_FLAG_REFLECTOR_CLIENT))
    vty_out (vty, "  Route-Reflector Client%s", VTY_NEWLINE);
  if (CHECK_FLAG (p->af_flags[afi][safi], PEER_FLAG_RSERVER_CLIENT))
    vty_out (vty, "  Route-Server Client%s", VTY_NEWLINE);
  if (CHECK_FLAG (p->af_flags[afi][safi], PEER_FLAG_SOFT_RECONFIG))
    vty_out (vty, "  Inbound soft reconfiguration allowed%s", VTY_NEWLINE);
  if (CHECK_FLAG (p->af_flags[afi][safi], PEER_FLAG_REMOVE_PRIVATE_AS))
    vty_out (vty, "  Private AS number removed from updates to this neighbor%s", VTY_NEWLINE);
  if (CHECK_FLAG (p->af_flags[afi][safi], PEER_FLAG_NEXTHOP_SELF))
    vty_out (vty, "  NEXT_HOP is always this router%s", VTY_NEWLINE);
  if (CHECK_FLAG (p->af_flags[afi][safi], PEER_FLAG_AS_PATH_UNCHANGED))
    vty_out (vty, "  AS_PATH is propagated unchanged to this neighbor%s", VTY_NEWLINE);
  if (CHECK_FLAG (p->af_flags[afi][safi], PEER_FLAG_NEXTHOP_UNCHANGED))
    vty_out (vty, "  NEXT_HOP is propagated unchanged to this neighbor%s", VTY_NEWLINE);
  if (CHECK_FLAG (p->af_flags[afi][safi], PEER_FLAG_MED_UNCHANGED))
    vty_out (vty, "  MED is propagated unchanged to this neighbor%s", VTY_NEWLINE);
  if (CHECK_FLAG (p->af_flags[afi][safi], PEER_FLAG_SEND_COMMUNITY)
      || CHECK_FLAG (p->af_flags[afi][safi], PEER_FLAG_SEND_EXT_COMMUNITY))
    {
      vty_out (vty, "  Community attribute sent to this neighbor");
      if (CHECK_FLAG (p->af_flags[afi][safi], PEER_FLAG_SEND_COMMUNITY)
	&& CHECK_FLAG (p->af_flags[afi][safi], PEER_FLAG_SEND_EXT_COMMUNITY))
	vty_out (vty, "(both)%s", VTY_NEWLINE);
      else if (CHECK_FLAG (p->af_flags[afi][safi], PEER_FLAG_SEND_EXT_COMMUNITY))
	vty_out (vty, "(extended)%s", VTY_NEWLINE);
      else 
	vty_out (vty, "(standard)%s", VTY_NEWLINE);
    }
  if (CHECK_FLAG (p->af_flags[afi][safi], PEER_FLAG_DEFAULT_ORIGINATE))
    {
      vty_out (vty, "  Default information originate,");

      if (p->default_rmap[afi][safi].name)
	vty_out (vty, " default route-map %s%s,",
		 p->default_rmap[afi][safi].map ? "*" : "",
		 p->default_rmap[afi][safi].name);
      if (CHECK_FLAG (p->af_sflags[afi][safi], PEER_STATUS_DEFAULT_ORIGINATE))
	vty_out (vty, " default sent%s", VTY_NEWLINE);
      else
	vty_out (vty, " default not sent%s", VTY_NEWLINE);
    }

  if (filter->plist[FILTER_IN].name
      || filter->dlist[FILTER_IN].name
      || filter->aslist[FILTER_IN].name
      || filter->map[RMAP_IN].name)
    vty_out (vty, "  Inbound path policy configured%s", VTY_NEWLINE);
  if (filter->plist[FILTER_OUT].name
      || filter->dlist[FILTER_OUT].name
      || filter->aslist[FILTER_OUT].name
      || filter->map[RMAP_OUT].name
      || filter->usmap.name)
    vty_out (vty, "  Outbound path policy configured%s", VTY_NEWLINE);
  if (filter->map[RMAP_IMPORT].name)
    vty_out (vty, "  Import policy for this RS-client configured%s", VTY_NEWLINE);
  if (filter->map[RMAP_EXPORT].name)
    vty_out (vty, "  Export policy for this RS-client configured%s", VTY_NEWLINE);

  /* prefix-list */
  if (filter->plist[FILTER_IN].name)
    vty_out (vty, "  Incoming update prefix filter list is %s%s%s",
	     filter->plist[FILTER_IN].plist ? "*" : "",
	     filter->plist[FILTER_IN].name,
	     VTY_NEWLINE);
  if (filter->plist[FILTER_OUT].name)
    vty_out (vty, "  Outgoing update prefix filter list is %s%s%s",
	     filter->plist[FILTER_OUT].plist ? "*" : "",
	     filter->plist[FILTER_OUT].name,
	     VTY_NEWLINE);

  /* distribute-list */
  if (filter->dlist[FILTER_IN].name)
    vty_out (vty, "  Incoming update network filter list is %s%s%s",
	     filter->dlist[FILTER_IN].alist ? "*" : "",
	     filter->dlist[FILTER_IN].name,
	     VTY_NEWLINE);
  if (filter->dlist[FILTER_OUT].name)
    vty_out (vty, "  Outgoing update network filter list is %s%s%s",
	     filter->dlist[FILTER_OUT].alist ? "*" : "",
	     filter->dlist[FILTER_OUT].name,
	     VTY_NEWLINE);

  /* filter-list. */
  if (filter->aslist[FILTER_IN].name)
    vty_out (vty, "  Incoming update AS path filter list is %s%s%s",
	     filter->aslist[FILTER_IN].aslist ? "*" : "",
	     filter->aslist[FILTER_IN].name,
	     VTY_NEWLINE);
  if (filter->aslist[FILTER_OUT].name)
    vty_out (vty, "  Outgoing update AS path filter list is %s%s%s",
	     filter->aslist[FILTER_OUT].aslist ? "*" : "",
	     filter->aslist[FILTER_OUT].name,
	     VTY_NEWLINE);

  /* route-map. */
  if (filter->map[RMAP_IN].name)
    vty_out (vty, "  Route map for incoming advertisements is %s%s%s",
            filter->map[RMAP_IN].map ? "*" : "",
            filter->map[RMAP_IN].name,
	     VTY_NEWLINE);
  if (filter->map[RMAP_OUT].name)
    vty_out (vty, "  Route map for outgoing advertisements is %s%s%s",
            filter->map[RMAP_OUT].map ? "*" : "",
            filter->map[RMAP_OUT].name,
            VTY_NEWLINE);
  if (filter->map[RMAP_IMPORT].name)
    vty_out (vty, "  Route map for advertisements going into this RS-client's table is %s%s%s",
            filter->map[RMAP_IMPORT].map ? "*" : "",
            filter->map[RMAP_IMPORT].name,
            VTY_NEWLINE);
  if (filter->map[RMAP_EXPORT].name)
    vty_out (vty, "  Route map for advertisements coming from this RS-client is %s%s%s",
            filter->map[RMAP_EXPORT].map ? "*" : "",
            filter->map[RMAP_EXPORT].name,
	     VTY_NEWLINE);

  /* unsuppress-map */
  if (filter->usmap.name)
    vty_out (vty, "  Route map for selective unsuppress is %s%s%s",
	     filter->usmap.map ? "*" : "",
	     filter->usmap.name, VTY_NEWLINE);

  /* Receive prefix count */
  vty_out (vty, "  %ld accepted prefixes%s", p->pcount[afi][safi], VTY_NEWLINE);

  /* Maximum prefix */
  if (CHECK_FLAG (p->af_flags[afi][safi], PEER_FLAG_MAX_PREFIX))
    {
      vty_out (vty, "  Maximum prefixes allowed %ld%s%s", p->pmax[afi][safi],
	       CHECK_FLAG (p->af_flags[afi][safi], PEER_FLAG_MAX_PREFIX_WARNING)
	       ? " (warning-only)" : "", VTY_NEWLINE);
      vty_out (vty, "  Threshold for warning message %d%%",
	       p->pmax_threshold[afi][safi]);
      if (p->pmax_restart[afi][safi])
	vty_out (vty, ", restart interval %d min", p->pmax_restart[afi][safi]);
      vty_out (vty, "%s", VTY_NEWLINE);
    }

  vty_out (vty, "%s", VTY_NEWLINE);
}