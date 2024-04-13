DEFUN (neighbor_allowas_in,
       neighbor_allowas_in_cmd,
       NEIGHBOR_CMD2 "allowas-in",
       NEIGHBOR_STR
       NEIGHBOR_ADDR_STR2
       "Accept as-path with my AS present in it\n")
{
  int ret;
  struct peer *peer;
  unsigned int allow_num;

  peer = peer_and_group_lookup_vty (vty, argv[0]);
  if (! peer)
    return CMD_WARNING;

  if (argc == 1)
    allow_num = 3;
  else
    VTY_GET_INTEGER_RANGE ("AS number", allow_num, argv[1], 1, 10);

  ret = peer_allowas_in_set (peer, bgp_node_afi (vty), bgp_node_safi (vty),
			     allow_num);

  return bgp_vty_return (vty, ret);
}