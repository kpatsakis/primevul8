DEFUN (show_ip_bgp_neighbors_peer,
       show_ip_bgp_neighbors_peer_cmd,
       "show ip bgp neighbors (A.B.C.D|X:X::X:X)",
       SHOW_STR
       IP_STR
       BGP_STR
       "Detailed information on TCP and BGP neighbor connections\n"
       "Neighbor to display information about\n"
       "Neighbor to display information about\n")
{
  return bgp_show_neighbor_vty (vty, NULL, show_peer, argv[argc - 1]);
}