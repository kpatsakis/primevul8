DEFUN (neighbor_update_source,
       neighbor_update_source_cmd,
       NEIGHBOR_CMD2 "update-source " BGP_UPDATE_SOURCE_STR,
       NEIGHBOR_STR
       NEIGHBOR_ADDR_STR2
       "Source of routing updates\n"
       BGP_UPDATE_SOURCE_HELP_STR)
{
  return peer_update_source_vty (vty, argv[0], argv[1]);
}