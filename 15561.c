DEFUN (bgp_bestpath_compare_router_id,
       bgp_bestpath_compare_router_id_cmd,
       "bgp bestpath compare-routerid",
       "BGP specific commands\n"
       "Change the default bestpath selection\n"
       "Compare router-id for identical EBGP paths\n")
{
  struct bgp *bgp;

  bgp = vty->index;
  bgp_flag_set (bgp, BGP_FLAG_COMPARE_ROUTER_ID);
  return CMD_SUCCESS;
}