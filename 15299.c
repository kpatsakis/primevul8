DEFUN (no_ip_community_list_name_standard_all,
       no_ip_community_list_name_standard_all_cmd,
       "no ip community-list standard WORD",
       NO_STR
       IP_STR
       COMMUNITY_LIST_STR
       "Add a standard community-list entry\n"
       "Community list name\n")
{
  return community_list_unset_vty (vty, argc, argv, COMMUNITY_LIST_STANDARD);
}