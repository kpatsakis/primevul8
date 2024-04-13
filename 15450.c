DEFUN (no_ip_extcommunity_list_name_expanded_all,
       no_ip_extcommunity_list_name_expanded_all_cmd,
       "no ip extcommunity-list expanded WORD",
       NO_STR
       IP_STR
       EXTCOMMUNITY_LIST_STR
       "Specify expanded extcommunity-list\n"
       "Extended Community list name\n")
{
  return extcommunity_list_unset_vty (vty, argc, argv, EXTCOMMUNITY_LIST_EXPANDED);
}