DEFUN (show_ip_extcommunity_list_arg,
       show_ip_extcommunity_list_arg_cmd,
       "show ip extcommunity-list (<1-500>|WORD)",
       SHOW_STR
       IP_STR
       "List extended-community list\n"
       "Extcommunity-list number\n"
       "Extcommunity-list name\n")
{
  struct community_list *list;

  list = community_list_lookup (bgp_clist, argv[0], EXTCOMMUNITY_LIST_MASTER);
  if (! list)
    {
      vty_out (vty, "%% Can't find extcommunit-list%s", VTY_NEWLINE);
      return CMD_WARNING;
    }

  extcommunity_list_show (vty, list);

  return CMD_SUCCESS;
}