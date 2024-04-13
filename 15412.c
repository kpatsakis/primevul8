DEFUN (no_neighbor_attr_unchanged2,
       no_neighbor_attr_unchanged2_cmd,
       NO_NEIGHBOR_CMD2 "attribute-unchanged as-path (next-hop|med)",
       NO_STR
       NEIGHBOR_STR
       NEIGHBOR_ADDR_STR2
       "BGP attribute is propagated unchanged to this neighbor\n"
       "As-path attribute\n"
       "Nexthop attribute\n"
       "Med attribute\n")
{
  u_int16_t flags = PEER_FLAG_AS_PATH_UNCHANGED;

  if (strncmp (argv[1], "next-hop", 1) == 0)
    SET_FLAG (flags, PEER_FLAG_NEXTHOP_UNCHANGED);
  else if (strncmp (argv[1], "med", 1) == 0)
    SET_FLAG (flags, PEER_FLAG_MED_UNCHANGED);

  return peer_af_flag_unset_vty (vty, argv[0], bgp_node_afi (vty),
			       bgp_node_safi (vty), flags);
}