DEFUN (neighbor_attr_unchanged3,
       neighbor_attr_unchanged3_cmd,
       NEIGHBOR_CMD2 "attribute-unchanged next-hop (as-path|med)",
       NEIGHBOR_STR
       NEIGHBOR_ADDR_STR2
       "BGP attribute is propagated unchanged to this neighbor\n"
       "Nexthop attribute\n"
       "As-path attribute\n"
       "Med attribute\n")
{
  u_int16_t flags = PEER_FLAG_NEXTHOP_UNCHANGED;

  if (strncmp (argv[1], "as-path", 1) == 0)
    SET_FLAG (flags, PEER_FLAG_AS_PATH_UNCHANGED);
  else if (strncmp (argv[1], "med", 1) == 0)
    SET_FLAG (flags, PEER_FLAG_MED_UNCHANGED);

  return peer_af_flag_set_vty (vty, argv[0], bgp_node_afi (vty),
			       bgp_node_safi (vty), flags);
}