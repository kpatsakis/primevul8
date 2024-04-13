DEFUN (bgp_config_type,
       bgp_config_type_cmd,
       "bgp config-type (cisco|zebra)",
       BGP_STR
       "Configuration type\n"
       "cisco\n"
       "zebra\n")
{
  if (strncmp (argv[0], "c", 1) == 0)
    bgp_option_set (BGP_OPT_CONFIG_CISCO);
  else
    bgp_option_unset (BGP_OPT_CONFIG_CISCO);

  return CMD_SUCCESS;
}