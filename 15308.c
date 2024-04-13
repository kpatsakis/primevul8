DEFUN (no_bgp_multiple_instance,
       no_bgp_multiple_instance_cmd,
       "no bgp multiple-instance",
       NO_STR
       BGP_STR
       "BGP multiple instance\n")
{
  int ret;

  ret = bgp_option_unset (BGP_OPT_MULTIPLE_INSTANCE);
  if (ret < 0)
    {
      vty_out (vty, "%% There are more than two BGP instances%s", VTY_NEWLINE);
      return CMD_WARNING;
    }
  return CMD_SUCCESS;
}