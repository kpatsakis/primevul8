_dbus_auth_dump_supported_mechanisms (DBusString *buffer)
{
  unsigned int i;
  _dbus_assert (buffer != NULL);

  for (i = 0; all_mechanisms[i].mechanism != NULL; i++)
    {
      if (i > 0)
        {
          if (!_dbus_string_append (buffer, ", "))
            return FALSE;
        }
      if (!_dbus_string_append (buffer, all_mechanisms[i].mechanism))
        return FALSE;
    }
  return TRUE;
}