DEFUN (neighbor_local_as,
       neighbor_local_as_cmd,
       NEIGHBOR_CMD2 "local-as <1-65535>",
       NEIGHBOR_STR
       NEIGHBOR_ADDR_STR2
       "Specify a local-as number\n"
       "AS number used as local AS\n")
{
  struct peer *peer;
  int ret;

  peer = peer_and_group_lookup_vty (vty, argv[0]);
  if (! peer)
    return CMD_WARNING;

  ret = peer_local_as_set (peer, atoi (argv[1]), 0);
  return bgp_vty_return (vty, ret);
}