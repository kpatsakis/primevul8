DEFUN (clear_ip_bgp_all,
       clear_ip_bgp_all_cmd,
       "clear ip bgp *",
       CLEAR_STR
       IP_STR
       BGP_STR
       "Clear all peers\n")
{
  if (argc == 1)
    return bgp_clear_vty (vty, argv[0], 0, 0, clear_all, BGP_CLEAR_SOFT_NONE, NULL);    

  return bgp_clear_vty (vty, NULL, 0, 0, clear_all, BGP_CLEAR_SOFT_NONE, NULL);
}