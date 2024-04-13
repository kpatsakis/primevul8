DEFUN (no_neighbor_ebgp_multihop,
       no_neighbor_ebgp_multihop_cmd,
       NO_NEIGHBOR_CMD2 "ebgp-multihop",
       NO_STR
       NEIGHBOR_STR
       NEIGHBOR_ADDR_STR2
       "Allow EBGP neighbors not on directly connected networks\n")
{
  return peer_ebgp_multihop_unset_vty (vty, argv[0]);
}