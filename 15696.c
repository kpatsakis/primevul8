DEFUN (bgp_multiple_instance_func,
       bgp_multiple_instance_cmd,
       "bgp multiple-instance",
       BGP_STR
       "Enable bgp multiple instance\n")
{
  bgp_option_set (BGP_OPT_MULTIPLE_INSTANCE);
  return CMD_SUCCESS;
}