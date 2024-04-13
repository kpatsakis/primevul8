DEFUN (ip_extcommunity_list_expanded,
       ip_extcommunity_list_expanded_cmd,
       "ip extcommunity-list <100-500> (deny|permit) .LINE",
       IP_STR
       EXTCOMMUNITY_LIST_STR
       "Extended Community list number (expanded)\n"
       "Specify community to reject\n"
       "Specify community to accept\n"
       "An ordered list as a regular-expression\n")
{
  return extcommunity_list_set_vty (vty, argc, argv, EXTCOMMUNITY_LIST_EXPANDED, 0);
}