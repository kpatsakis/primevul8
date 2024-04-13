DEFUN (show_ip_bgp_instance_neighbors,
       show_ip_bgp_instance_neighbors_cmd,
       "show ip bgp view WORD neighbors",
       SHOW_STR
       IP_STR
       BGP_STR
       "BGP view\n"
       "View name\n"
       "Detailed information on TCP and BGP neighbor connections\n")
{
  return bgp_show_neighbor_vty (vty, argv[0], show_all, NULL);
}