DEFUN (bgp_client_to_client_reflection,
       bgp_client_to_client_reflection_cmd,
       "bgp client-to-client reflection",
       "BGP specific commands\n"
       "Configure client to client route reflection\n"
       "reflection of routes allowed\n")
{
  struct bgp *bgp;

  bgp = vty->index;
  bgp_flag_unset (bgp, BGP_FLAG_NO_CLIENT_TO_CLIENT);
  return CMD_SUCCESS;
}