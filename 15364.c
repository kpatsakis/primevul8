DEFUN (no_neighbor_update_source,
       no_neighbor_update_source_cmd,
       NO_NEIGHBOR_CMD2 "update-source",
       NO_STR
       NEIGHBOR_STR
       NEIGHBOR_ADDR_STR2
       "Source of routing updates\n")
{
  return peer_update_source_vty (vty, argv[0], NULL);
}