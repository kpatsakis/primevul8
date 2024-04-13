DEFUN (show_ip_bgp_instance_neighbors_peer,
       show_ip_bgp_instance_neighbors_peer_cmd,
       "show ip bgp view WORD neighbors (A.B.C.D|X:X::X:X)",
       SHOW_STR
       IP_STR
       BGP_STR
       "BGP view\n"
       "View name\n"
       "Detailed information on TCP and BGP neighbor connections\n"
       "Neighbor to display information about\n"
       "Neighbor to display information about\n")
{
  return bgp_show_neighbor_vty (vty, argv[0], show_peer, argv[1]);
}