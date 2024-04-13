DEFUN (no_neighbor_weight,
       no_neighbor_weight_cmd,
       NO_NEIGHBOR_CMD2 "weight",
       NO_STR
       NEIGHBOR_STR
       NEIGHBOR_ADDR_STR2
       "Set default weight for routes from this neighbor\n")
{
  return peer_weight_unset_vty (vty, argv[0]);
}