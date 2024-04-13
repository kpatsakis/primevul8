DEFUN (neighbor_activate,
       neighbor_activate_cmd,
       NEIGHBOR_CMD2 "activate",
       NEIGHBOR_STR
       NEIGHBOR_ADDR_STR2
       "Enable the Address Family for this Neighbor\n")
{
  struct peer *peer;

  peer = peer_and_group_lookup_vty (vty, argv[0]);
  if (! peer)
    return CMD_WARNING;

  peer_activate (peer, bgp_node_afi (vty), bgp_node_safi (vty));

  return CMD_SUCCESS;
}