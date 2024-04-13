DEFUN (no_neighbor_description,
       no_neighbor_description_cmd,
       NO_NEIGHBOR_CMD2 "description",
       NO_STR
       NEIGHBOR_STR
       NEIGHBOR_ADDR_STR2
       "Neighbor specific description\n")
{
  struct peer *peer;

  peer = peer_and_group_lookup_vty (vty, argv[0]);
  if (! peer)
    return CMD_WARNING;

  peer_description_unset (peer);

  return CMD_SUCCESS;
}