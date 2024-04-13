community_list_config_str (struct community_entry *entry)
{
  const char *str;

  if (entry->any)
    str = "";
  else
    {
      if (entry->style == COMMUNITY_LIST_STANDARD)
	str = community_str (entry->u.com);
      else
	str = entry->config;
    }
  return str;
}