community_list_config_write (struct vty *vty)
{
  struct community_list *list;
  struct community_entry *entry;
  struct community_list_master *cm;
  int write = 0;

  /* Community-list.  */
  cm = community_list_master_lookup (bgp_clist, COMMUNITY_LIST_MASTER);

  for (list = cm->num.head; list; list = list->next)
    for (entry = list->head; entry; entry = entry->next)
      {
	vty_out (vty, "ip community-list %s %s %s%s",
		 list->name, community_direct_str (entry->direct),
		 community_list_config_str (entry),
		 VTY_NEWLINE);
	write++;
      }
  for (list = cm->str.head; list; list = list->next)
    for (entry = list->head; entry; entry = entry->next)
      {
	vty_out (vty, "ip community-list %s %s %s %s%s",
		 entry->style == COMMUNITY_LIST_STANDARD
		 ? "standard" : "expanded",
		 list->name, community_direct_str (entry->direct),
		 community_list_config_str (entry),
		 VTY_NEWLINE);
	write++;
      }

  /* Extcommunity-list.  */
  cm = community_list_master_lookup (bgp_clist, EXTCOMMUNITY_LIST_MASTER);

  for (list = cm->num.head; list; list = list->next)
    for (entry = list->head; entry; entry = entry->next)
      {
	vty_out (vty, "ip extcommunity-list %s %s %s%s",
		 list->name, community_direct_str (entry->direct),
		 community_list_config_str (entry), VTY_NEWLINE);
	write++;
      }
  for (list = cm->str.head; list; list = list->next)
    for (entry = list->head; entry; entry = entry->next)
      {
	vty_out (vty, "ip extcommunity-list %s %s %s %s%s",
		 entry->style == EXTCOMMUNITY_LIST_STANDARD
		 ? "standard" : "expanded",
		 list->name, community_direct_str (entry->direct),
		 community_list_config_str (entry), VTY_NEWLINE);
	write++;
      }
  return write;
}