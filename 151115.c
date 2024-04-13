flatpak_context_device_from_string (const char *string, GError **error)
{
  FlatpakContextDevices devices = flatpak_context_bitmask_from_string (string, flatpak_context_devices);

  if (devices == 0)
    {
      g_autofree char *values = g_strjoinv (", ", (char **)flatpak_context_devices);
      g_set_error (error, G_OPTION_ERROR, G_OPTION_ERROR_FAILED,
                   _("Unknown device type %s, valid types are: %s"), string, values);
    }
  return devices;
}