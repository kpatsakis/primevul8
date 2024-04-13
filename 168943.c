process_data (DBusAuth             *auth,
              const DBusString     *args,
              DBusAuthDataFunction  data_func)
{
  int end;
  DBusString decoded;

  if (!_dbus_string_init (&decoded))
    return FALSE;

  if (!_dbus_string_hex_decode (args, 0, &end, &decoded, 0))
    {
      _dbus_string_free (&decoded);
      return FALSE;
    }

  if (_dbus_string_get_length (args) != end)
    {
      _dbus_string_free (&decoded);
      if (!send_error (auth, "Invalid hex encoding"))
        return FALSE;

      return TRUE;
    }

#ifdef DBUS_ENABLE_VERBOSE_MODE
  if (_dbus_string_validate_ascii (&decoded, 0,
                                   _dbus_string_get_length (&decoded)))
    _dbus_verbose ("%s: data: '%s'\n",
                   DBUS_AUTH_NAME (auth),
                   _dbus_string_get_const_data (&decoded));
#endif
      
  if (!(* data_func) (auth, &decoded))
    {
      _dbus_string_free (&decoded);
      return FALSE;
    }

  _dbus_string_free (&decoded);
  return TRUE;
}