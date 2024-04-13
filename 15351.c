DEFUN (show_ip_community_list_arg,
       show_ip_community_list_arg_cmd,
       "show ip community-list (<1-500>|WORD)",
       SHOW_STR
       IP_STR
       "List community-list\n"
       "Community-list number\n"
       "Community-list name\n")
{
  struct community_list *list;

  list = community_list_lookup (bgp_clist, argv[0], COMMUNITY_LIST_MASTER);
  if (! list)
    {
      vty_out (vty, "%% Can't find communit-list%s", VTY_NEWLINE);
      return CMD_WARNING;
    }

  community_list_show (vty, list);

  return CMD_SUCCESS;
}