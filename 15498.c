DEFUN (show_ip_extcommunity_list,
       show_ip_extcommunity_list_cmd,
       "show ip extcommunity-list",
       SHOW_STR
       IP_STR
       "List extended-community list\n")
{
  struct community_list *list;
  struct community_list_master *cm;

  cm = community_list_master_lookup (bgp_clist, EXTCOMMUNITY_LIST_MASTER);
  if (! cm)
    return CMD_SUCCESS;

  for (list = cm->num.head; list; list = list->next)
    extcommunity_list_show (vty, list);

  for (list = cm->str.head; list; list = list->next)
    extcommunity_list_show (vty, list);

  return CMD_SUCCESS;
}