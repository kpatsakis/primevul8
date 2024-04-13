DEFUN (ip_extcommunity_list_standard,
       ip_extcommunity_list_standard_cmd,
       "ip extcommunity-list <1-99> (deny|permit) .AA:NN",
       IP_STR
       EXTCOMMUNITY_LIST_STR
       "Extended Community list number (standard)\n"
       "Specify community to reject\n"
       "Specify community to accept\n"
       EXTCOMMUNITY_VAL_STR)
{
  return extcommunity_list_set_vty (vty, argc, argv, EXTCOMMUNITY_LIST_STANDARD, 0);
}