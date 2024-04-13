DEFUN (no_neighbor_activate,
       no_neighbor_activate_cmd,
       NO_NEIGHBOR_CMD2 "activate",
       NO_STR
       NEIGHBOR_STR
       NEIGHBOR_ADDR_STR2
       "Enable the Address Family for this Neighbor\n")
{
  int ret;
  struct peer *peer;

  /* Lookup peer. */
  peer = peer_and_group_lookup_vty (vty, argv[0]);
  if (! peer)
    return CMD_WARNING;

  ret = peer_deactivate (peer, bgp_node_afi (vty), bgp_node_safi (vty));

  return bgp_vty_return (vty, ret);
}