DEFUN (no_ip_extcommunity_list_standard_all,
       no_ip_extcommunity_list_standard_all_cmd,
       "no ip extcommunity-list <1-99>",
       NO_STR
       IP_STR
       EXTCOMMUNITY_LIST_STR
       "Extended Community list number (standard)\n")
{
  return extcommunity_list_unset_vty (vty, argc, argv, EXTCOMMUNITY_LIST_STANDARD);
}