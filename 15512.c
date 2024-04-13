DEFUN (bgp_fast_external_failover,
       bgp_fast_external_failover_cmd,
       "bgp fast-external-failover",
       BGP_STR
       "Immediately reset session if a link to a directly connected external peer goes down\n")
{
  struct bgp *bgp;

  bgp = vty->index;
  bgp_flag_unset (bgp, BGP_FLAG_NO_FAST_EXT_FAILOVER);
  return CMD_SUCCESS;
}