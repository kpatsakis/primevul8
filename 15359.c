DEFUN (ip_community_list_name_standard,
       ip_community_list_name_standard_cmd,
       "ip community-list standard WORD (deny|permit) .AA:NN",
       IP_STR
       COMMUNITY_LIST_STR
       "Add a standard community-list entry\n"
       "Community list name\n"
       "Specify community to reject\n"
       "Specify community to accept\n"
       COMMUNITY_VAL_STR)
{
  return community_list_set_vty (vty, argc, argv, COMMUNITY_LIST_STANDARD, 1);
}