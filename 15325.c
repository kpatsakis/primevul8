extcommunity_list_show (struct vty *vty, struct community_list *list)
{
  struct community_entry *entry;

  for (entry = list->head; entry; entry = entry->next)
    {
      if (entry == list->head)
	{
	  if (all_digit (list->name))
	    vty_out (vty, "Extended community %s list %s%s",
		     entry->style == EXTCOMMUNITY_LIST_STANDARD ?
		     "standard" : "(expanded) access",
		     list->name, VTY_NEWLINE);
	  else
	    vty_out (vty, "Named extended community %s list %s%s",
		     entry->style == EXTCOMMUNITY_LIST_STANDARD ?
		     "standard" : "expanded",
		     list->name, VTY_NEWLINE);
	}
      if (entry->any)
	vty_out (vty, "    %s%s",
		 community_direct_str (entry->direct), VTY_NEWLINE);
      else
	vty_out (vty, "    %s %s%s",
		 community_direct_str (entry->direct),
		 entry->style == EXTCOMMUNITY_LIST_STANDARD ?
		 entry->u.ecom->str : entry->config,
		 VTY_NEWLINE);
    }
}