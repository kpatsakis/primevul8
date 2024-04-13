DEFUN (neighbor_default_originate_rmap,
       neighbor_default_originate_rmap_cmd,
       NEIGHBOR_CMD2 "default-originate route-map WORD",
       NEIGHBOR_STR
       NEIGHBOR_ADDR_STR2
       "Originate default route to this neighbor\n"
       "Route-map to specify criteria to originate default\n"
       "route-map name\n")
{
  return peer_default_originate_set_vty (vty, argv[0], bgp_node_afi (vty),
					 bgp_node_safi (vty), argv[1], 1);
}