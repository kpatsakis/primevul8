DEFUN (neighbor_weight,
       neighbor_weight_cmd,
       NEIGHBOR_CMD2 "weight <0-65535>",
       NEIGHBOR_STR
       NEIGHBOR_ADDR_STR2
       "Set default weight for routes from this neighbor\n"
       "default weight\n")
{
  return peer_weight_set_vty (vty, argv[0], argv[1]);
}