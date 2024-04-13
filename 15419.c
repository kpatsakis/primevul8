DEFUN (no_neighbor_attr_unchanged4,
       no_neighbor_attr_unchanged4_cmd,
       NO_NEIGHBOR_CMD2 "attribute-unchanged med (as-path|next-hop)",
       NO_STR
       NEIGHBOR_STR
       NEIGHBOR_ADDR_STR2
       "BGP attribute is propagated unchanged to this neighbor\n"
       "Med attribute\n"
       "As-path attribute\n"
       "Nexthop attribute\n")
{
  u_int16_t flags = PEER_FLAG_MED_UNCHANGED;

  if (strncmp (argv[1], "as-path", 1) == 0)
    SET_FLAG (flags, PEER_FLAG_AS_PATH_UNCHANGED);
  else if (strncmp (argv[1], "next-hop", 1) == 0)
    SET_FLAG (flags, PEER_FLAG_NEXTHOP_UNCHANGED);

  return peer_af_flag_unset_vty (vty, argv[0], bgp_node_afi (vty),
			       bgp_node_safi (vty), flags);
}