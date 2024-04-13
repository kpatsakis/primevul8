DEFUN (neighbor_ebgp_multihop_ttl,
       neighbor_ebgp_multihop_ttl_cmd,
       NEIGHBOR_CMD2 "ebgp-multihop <1-255>",
       NEIGHBOR_STR
       NEIGHBOR_ADDR_STR2
       "Allow EBGP neighbors not on directly connected networks\n"
       "maximum hop count\n")
{
  return peer_ebgp_multihop_set_vty (vty, argv[0], argv[1]);
}