flatpak_verify_dbus_name (const char *name, GError **error)
{
  const char *name_part;
  g_autofree char *tmp = NULL;

  if (g_str_has_suffix (name, ".*"))
    {
      tmp = g_strndup (name, strlen (name) - 2);
      name_part = tmp;
    }
  else
    {
      name_part = name;
    }

  if (g_dbus_is_name (name_part) && !g_dbus_is_unique_name (name_part))
    return TRUE;

  g_set_error (error, G_OPTION_ERROR, G_OPTION_ERROR_FAILED,
               _("Invalid dbus name %s\n"), name);
  return FALSE;
}