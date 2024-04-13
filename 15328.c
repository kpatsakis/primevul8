DEFUN (neighbor_default_originate,
       neighbor_default_originate_cmd,
       NEIGHBOR_CMD2 "default-originate",
       NEIGHBOR_STR
       NEIGHBOR_ADDR_STR2
       "Originate default route to this neighbor\n")
{
  return peer_default_originate_set_vty (vty, argv[0], bgp_node_afi (vty),
					 bgp_node_safi (vty), NULL, 1);
}