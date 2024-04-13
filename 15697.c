DEFUN (no_bgp_fast_external_failover,
       no_bgp_fast_external_failover_cmd,
       "no bgp fast-external-failover",
       NO_STR
       BGP_STR
       "Immediately reset session if a link to a directly connected external peer goes down\n")
{
  struct bgp *bgp;

  bgp = vty->index;
  bgp_flag_set (bgp, BGP_FLAG_NO_FAST_EXT_FAILOVER);
  return CMD_SUCCESS;
}