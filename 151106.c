get_xdg_dir_from_prefix (const char *prefix,
                         const char **where,
                         const char **dir)
{
  if (strcmp (prefix, "xdg-data") == 0)
    {
      if (where)
        *where = "data";
      if (dir)
        *dir = g_get_user_data_dir ();
      return TRUE;
    }
  if (strcmp (prefix, "xdg-cache") == 0)
    {
      if (where)
        *where = "cache";
      if (dir)
        *dir = g_get_user_cache_dir ();
      return TRUE;
    }
  if (strcmp (prefix, "xdg-config") == 0)
    {
      if (where)
        *where = "config";
      if (dir)
        *dir = g_get_user_config_dir ();
      return TRUE;
    }
  return FALSE;
}