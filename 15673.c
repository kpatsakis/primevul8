DEFUN (clear_bgp_peer_group_soft_out,
       clear_bgp_peer_group_soft_out_cmd,
       "clear bgp peer-group WORD soft out",
       CLEAR_STR
       BGP_STR
       "Clear all members of peer-group\n"
       "BGP peer-group name\n"
       "Soft reconfig\n"
       "Soft reconfig outbound update\n")
{
  return bgp_clear_vty (vty, NULL, AFI_IP6, SAFI_UNICAST, clear_group,
			BGP_CLEAR_SOFT_OUT, argv[0]);
}