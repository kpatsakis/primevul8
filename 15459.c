DEFUN (no_ip_community_list_standard_all,
       no_ip_community_list_standard_all_cmd,
       "no ip community-list <1-99>",
       NO_STR
       IP_STR
       COMMUNITY_LIST_STR
       "Community list number (standard)\n")
{
  return community_list_unset_vty (vty, argc, argv, COMMUNITY_LIST_STANDARD);
}