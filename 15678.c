DEFUN (no_ip_community_list_expanded_all,
       no_ip_community_list_expanded_all_cmd,
       "no ip community-list <100-500>",
       NO_STR
       IP_STR
       COMMUNITY_LIST_STR
       "Community list number (expanded)\n")
{
  return community_list_unset_vty (vty, argc, argv, COMMUNITY_LIST_EXPANDED);
}