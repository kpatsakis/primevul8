lookup_command_from_name (DBusString *command)
{
  int i;

  for (i = 0; i < _DBUS_N_ELEMENTS (auth_command_names); i++)
    {
      if (_dbus_string_equal_c_str (command,
                                    auth_command_names[i].name))
        return auth_command_names[i].command;
    }

  return DBUS_AUTH_COMMAND_UNKNOWN;
}