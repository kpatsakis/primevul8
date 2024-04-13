DEFUN (no_ip_community_list_standard,
       no_ip_community_list_standard_cmd,
       "no ip community-list <1-99> (deny|permit) .AA:NN",
       NO_STR
       IP_STR
       COMMUNITY_LIST_STR
       "Community list number (standard)\n"
       "Specify community to reject\n"
       "Specify community to accept\n"
       COMMUNITY_VAL_STR)
{
  return community_list_unset_vty (vty, argc, argv, COMMUNITY_LIST_STANDARD);
}