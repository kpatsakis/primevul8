DEFUN (no_ip_community_list_expanded,
       no_ip_community_list_expanded_cmd,
       "no ip community-list <100-500> (deny|permit) .LINE",
       NO_STR
       IP_STR
       COMMUNITY_LIST_STR
       "Community list number (expanded)\n"
       "Specify community to reject\n"
       "Specify community to accept\n"
       "An ordered list as a regular-expression\n")
{
  return community_list_unset_vty (vty, argc, argv, COMMUNITY_LIST_EXPANDED);
}