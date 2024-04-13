DEFUN (no_ip_community_list_name_expanded,
       no_ip_community_list_name_expanded_cmd,
       "no ip community-list expanded WORD (deny|permit) .LINE",
       NO_STR
       IP_STR
       COMMUNITY_LIST_STR
       "Specify an expanded community-list\n"
       "Community list name\n"
       "Specify community to reject\n"
       "Specify community to accept\n"
       "An ordered list as a regular-expression\n")
{
  return community_list_unset_vty (vty, argc, argv, COMMUNITY_LIST_EXPANDED);
}