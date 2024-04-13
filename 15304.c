DEFUN (no_bgp_config_type,
       no_bgp_config_type_cmd,
       "no bgp config-type",
       NO_STR
       BGP_STR
       "Display configuration type\n")
{
  bgp_option_unset (BGP_OPT_CONFIG_CISCO);
  return CMD_SUCCESS;
}