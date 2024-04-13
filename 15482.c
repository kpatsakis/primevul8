DEFUN (ip_extcommunity_list_name_expanded,
       ip_extcommunity_list_name_expanded_cmd,
       "ip extcommunity-list expanded WORD (deny|permit) .LINE",
       IP_STR
       EXTCOMMUNITY_LIST_STR
       "Specify expanded extcommunity-list\n"
       "Extended Community list name\n"
       "Specify community to reject\n"
       "Specify community to accept\n"
       "An ordered list as a regular-expression\n")
{
  return extcommunity_list_set_vty (vty, argc, argv, EXTCOMMUNITY_LIST_EXPANDED, 1);
}