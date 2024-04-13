DEFUN (no_bgp_network_import_check,
       no_bgp_network_import_check_cmd,
       "no bgp network import-check",
       NO_STR
       "BGP specific commands\n"
       "BGP network command\n"
       "Check BGP network route exists in IGP\n")
{
  struct bgp *bgp;

  bgp = vty->index;
  bgp_flag_unset (bgp, BGP_FLAG_IMPORT_CHECK);
  return CMD_SUCCESS;
}