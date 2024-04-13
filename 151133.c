get_xdg_dir_from_string (const char *filesystem,
                         const char **suffix,
                         const char **where)
{
  char *slash;
  const char *rest;
  g_autofree char *prefix;
  const char *dir = NULL;
  gsize len;

  slash = strchr (filesystem, '/');

  if (slash)
    len = slash - filesystem;
  else
    len = strlen (filesystem);

  rest = filesystem + len;
  while (*rest == '/')
    rest++;

  if (suffix != NULL)
    *suffix = rest;

  prefix = g_strndup (filesystem, len);

  if (get_xdg_dir_from_prefix (prefix, where, &dir))
    return g_build_filename (dir, rest, NULL);

  return NULL;
}