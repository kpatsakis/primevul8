DEFUN (no_ip_extcommunity_list_expanded_all,
       no_ip_extcommunity_list_expanded_all_cmd,
       "no ip extcommunity-list <100-500>",
       NO_STR
       IP_STR
       EXTCOMMUNITY_LIST_STR
       "Extended Community list number (expanded)\n")
{
  return extcommunity_list_unset_vty (vty, argc, argv, EXTCOMMUNITY_LIST_EXPANDED);
}