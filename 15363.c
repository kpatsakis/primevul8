DEFUN (neighbor_ebgp_multihop,
       neighbor_ebgp_multihop_cmd,
       NEIGHBOR_CMD2 "ebgp-multihop",
       NEIGHBOR_STR
       NEIGHBOR_ADDR_STR2
       "Allow EBGP neighbors not on directly connected networks\n")
{
  return peer_ebgp_multihop_set_vty (vty, argv[0], NULL);
}