DEFUN (ip_community_list_name_expanded,
       ip_community_list_name_expanded_cmd,
       "ip community-list expanded WORD (deny|permit) .LINE",
       IP_STR
       COMMUNITY_LIST_STR
       "Add an expanded community-list entry\n"
       "Community list name\n"
       "Specify community to reject\n"
       "Specify community to accept\n"
       "An ordered list as a regular-expression\n")
{
  return community_list_set_vty (vty, argc, argv, COMMUNITY_LIST_EXPANDED, 1);
}