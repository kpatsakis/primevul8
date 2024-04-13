community_list_show (struct vty *vty, struct community_list *list)
{
  struct community_entry *entry;

  for (entry = list->head; entry; entry = entry->next)
    {
      if (entry == list->head)
	{
	  if (all_digit (list->name))
	    vty_out (vty, "Community %s list %s%s",
		     entry->style == COMMUNITY_LIST_STANDARD ?
		     "standard" : "(expanded) access",
		     list->name, VTY_NEWLINE);
	  else
	    vty_out (vty, "Named Community %s list %s%s",
		     entry->style == COMMUNITY_LIST_STANDARD ?
		     "standard" : "expanded",
		     list->name, VTY_NEWLINE);
	}
      if (entry->any)
	vty_out (vty, "    %s%s",
		 community_direct_str (entry->direct), VTY_NEWLINE);
      else
	vty_out (vty, "    %s %s%s",
		 community_direct_str (entry->direct),
		 entry->style == COMMUNITY_LIST_STANDARD
		 ? community_str (entry->u.com) : entry->config,
		 VTY_NEWLINE);
    }
}