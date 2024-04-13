flatpak_context_verify_filesystem (const char *filesystem_and_mode,
                                   GError    **error)
{
  g_autofree char *filesystem = parse_filesystem_flags (filesystem_and_mode, NULL);

  if (strcmp (filesystem, "host") == 0)
    return TRUE;
  if (strcmp (filesystem, "home") == 0)
    return TRUE;
  if (get_xdg_user_dir_from_string (filesystem, NULL, NULL, NULL))
    return TRUE;
  if (g_str_has_prefix (filesystem, "~/"))
    return TRUE;
  if (g_str_has_prefix (filesystem, "/"))
    return TRUE;

  g_set_error (error, G_OPTION_ERROR, G_OPTION_ERROR_FAILED,
               _("Unknown filesystem location %s, valid types are: host, home, xdg-*[/...], ~/dir, /dir"), filesystem);
  return FALSE;
}