parse_filesystem_flags (const char *filesystem, FlatpakFilesystemMode *mode)
{
  gsize len = strlen (filesystem);

  if (mode)
    *mode = FLATPAK_FILESYSTEM_MODE_READ_WRITE;

  if (g_str_has_suffix (filesystem, ":ro"))
    {
      len -= 3;
      if (mode)
        *mode = FLATPAK_FILESYSTEM_MODE_READ_ONLY;
    }
  else if (g_str_has_suffix (filesystem, ":rw"))
    {
      len -= 3;
      if (mode)
        *mode = FLATPAK_FILESYSTEM_MODE_READ_WRITE;
    }
  else if (g_str_has_suffix (filesystem, ":create"))
    {
      len -= 7;
      if (mode)
        *mode = FLATPAK_FILESYSTEM_MODE_CREATE;
    }

  return g_strndup (filesystem, len);
}