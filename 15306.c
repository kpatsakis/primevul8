DEFUN (bgp_default_ipv4_unicast,
       bgp_default_ipv4_unicast_cmd,
       "bgp default ipv4-unicast",
       "BGP specific commands\n"
       "Configure BGP defaults\n"
       "Activate ipv4-unicast for a peer by default\n")
{
  struct bgp *bgp;

  bgp = vty->index;
  bgp_flag_unset (bgp, BGP_FLAG_NO_DEFAULT_IPV4);
  return CMD_SUCCESS;
}