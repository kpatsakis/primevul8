DEFUN (no_ip_extcommunity_list_name_standard,
       no_ip_extcommunity_list_name_standard_cmd,
       "no ip extcommunity-list standard WORD (deny|permit) .AA:NN",
       NO_STR
       IP_STR
       EXTCOMMUNITY_LIST_STR
       "Specify standard extcommunity-list\n"
       "Extended Community list name\n"
       "Specify community to reject\n"
       "Specify community to accept\n"
       EXTCOMMUNITY_VAL_STR)
{
  return extcommunity_list_unset_vty (vty, argc, argv, EXTCOMMUNITY_LIST_STANDARD);
}