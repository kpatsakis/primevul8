DEFUN (no_ip_extcommunity_list_name_expanded,
       no_ip_extcommunity_list_name_expanded_cmd,
       "no ip extcommunity-list expanded WORD (deny|permit) .LINE",
       NO_STR
       IP_STR
       EXTCOMMUNITY_LIST_STR
       "Specify expanded extcommunity-list\n"
       "Community list name\n"
       "Specify community to reject\n"
       "Specify community to accept\n"
       "An ordered list as a regular-expression\n")
{
  return extcommunity_list_unset_vty (vty, argc, argv, EXTCOMMUNITY_LIST_EXPANDED);
}