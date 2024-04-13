peer_rsclient_unset_vty (struct vty *vty, const char *peer_str, 
                         int afi, int safi)
{
  int ret;
  struct bgp *bgp;
  struct peer *peer;
  struct peer_group *group;
  struct listnode *node, *nnode;

  bgp = vty->index;

  peer = peer_and_group_lookup_vty (vty, peer_str);
  if ( ! peer )
    return CMD_WARNING;

  /* If it is not a RS-Client, don't do anything. */
  if ( ! CHECK_FLAG (peer->af_flags[afi][safi], PEER_FLAG_RSERVER_CLIENT) )
    return CMD_SUCCESS;

  if (CHECK_FLAG(peer->sflags, PEER_STATUS_GROUP))
    {
      group = peer->group;

      for (ALL_LIST_ELEMENTS (group->peer, node, nnode, peer))
        {
          ret = peer_af_flag_unset (peer, afi, safi, PEER_FLAG_RSERVER_CLIENT);
          if (ret < 0)
            return bgp_vty_return (vty, ret);

          peer->rib[afi][safi] = NULL;
        }

        peer = group->conf;
    }

  ret = peer_af_flag_unset (peer, afi, safi, PEER_FLAG_RSERVER_CLIENT);
  if (ret < 0)
    return bgp_vty_return (vty, ret);

  if ( ! peer_rsclient_active (peer) )
    {
      peer_unlock (peer); /* peer bgp rsclient reference */
      listnode_delete (bgp->rsclient, peer);
    }

  bgp_table_finish (peer->rib[bgp_node_afi(vty)][bgp_node_safi(vty)]);

  return CMD_SUCCESS;
}