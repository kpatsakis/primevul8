peer_rsclient_set_vty (struct vty *vty, const char *peer_str, 
                       int afi, int safi)
{
  int ret;
  struct bgp *bgp;
  struct peer *peer;
  struct peer_group *group;
  struct listnode *node, *nnode;
  struct bgp_filter *pfilter;
  struct bgp_filter *gfilter;

  bgp = vty->index;

  peer = peer_and_group_lookup_vty (vty, peer_str);
  if ( ! peer )
    return CMD_WARNING;

  /* If it is already a RS-Client, don't do anything. */
  if ( CHECK_FLAG (peer->af_flags[afi][safi], PEER_FLAG_RSERVER_CLIENT) )
    return CMD_SUCCESS;

  if ( ! peer_rsclient_active (peer) )
    {
      peer = peer_lock (peer); /* rsclient peer list reference */
      listnode_add_sort (bgp->rsclient, peer);
    }

  ret = peer_af_flag_set (peer, afi, safi, PEER_FLAG_RSERVER_CLIENT);
  if (ret < 0)
    return bgp_vty_return (vty, ret);

  peer->rib[afi][safi] = bgp_table_init (afi, safi);
  peer->rib[afi][safi]->type = BGP_TABLE_RSCLIENT;
  peer->rib[afi][safi]->owner = peer;

  /* Check for existing 'network' and 'redistribute' routes. */
  bgp_check_local_routes_rsclient (peer, afi, safi);

  /* Check for routes for peers configured with 'soft-reconfiguration'. */
  bgp_soft_reconfig_rsclient (peer, afi, safi);

  if (CHECK_FLAG(peer->sflags, PEER_STATUS_GROUP))
    {
      group = peer->group;
      gfilter = &peer->filter[afi][safi];

      for (ALL_LIST_ELEMENTS (group->peer, node, nnode, peer))
        {
          pfilter = &peer->filter[afi][safi];

          /* Members of a non-RS-Client group should not be RS-Clients, as that 
             is checked when the become part of the peer-group */
          ret = peer_af_flag_set (peer, afi, safi, PEER_FLAG_RSERVER_CLIENT);
          if (ret < 0)
            return bgp_vty_return (vty, ret);

          /* Make peer's RIB point to group's RIB. */
          peer->rib[afi][safi] = group->conf->rib[afi][safi];

          /* Import policy. */
          if (pfilter->map[RMAP_IMPORT].name)
            free (pfilter->map[RMAP_IMPORT].name);
          if (gfilter->map[RMAP_IMPORT].name)
            {
              pfilter->map[RMAP_IMPORT].name = strdup (gfilter->map[RMAP_IMPORT].name);
              pfilter->map[RMAP_IMPORT].map = gfilter->map[RMAP_IMPORT].map;
            }
          else
            {
              pfilter->map[RMAP_IMPORT].name = NULL;
              pfilter->map[RMAP_IMPORT].map =NULL;
            }

          /* Export policy. */
          if (gfilter->map[RMAP_EXPORT].name && ! pfilter->map[RMAP_EXPORT].name)
            {
              pfilter->map[RMAP_EXPORT].name = strdup (gfilter->map[RMAP_EXPORT].name);
              pfilter->map[RMAP_EXPORT].map = gfilter->map[RMAP_EXPORT].map;
            }
        }
    }
  return CMD_SUCCESS;
}