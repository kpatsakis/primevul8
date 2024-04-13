DEFUN (no_ip_community_list_name_standard,
       no_ip_community_list_name_standard_cmd,
       "no ip community-list standard WORD (deny|permit) .AA:NN",
       NO_STR
       IP_STR
       COMMUNITY_LIST_STR
       "Specify a standard community-list\n"
       "Community list name\n"
       "Specify community to reject\n"
       "Specify community to accept\n"
       COMMUNITY_VAL_STR)
{
  return community_list_unset_vty (vty, argc, argv, COMMUNITY_LIST_STANDARD);
}