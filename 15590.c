DEFUN (no_neighbor_allowas_in,
       no_neighbor_allowas_in_cmd,
       NO_NEIGHBOR_CMD2 "allowas-in",
       NO_STR
       NEIGHBOR_STR
       NEIGHBOR_ADDR_STR2
       "allow local ASN appears in aspath attribute\n")
{
  int ret;
  struct peer *peer;

  peer = peer_and_group_lookup_vty (vty, argv[0]);
  if (! peer)
    return CMD_WARNING;

  ret = peer_allowas_in_unset (peer, bgp_node_afi (vty), bgp_node_safi (vty));

  return bgp_vty_return (vty, ret);
}