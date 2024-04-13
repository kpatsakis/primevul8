DEFUN (no_bgp_bestpath_compare_router_id,
       no_bgp_bestpath_compare_router_id_cmd,
       "no bgp bestpath compare-routerid",
       NO_STR
       "BGP specific commands\n"
       "Change the default bestpath selection\n"
       "Compare router-id for identical EBGP paths\n")
{
  struct bgp *bgp;

  bgp = vty->index;
  bgp_flag_unset (bgp, BGP_FLAG_COMPARE_ROUTER_ID);
  return CMD_SUCCESS;
}