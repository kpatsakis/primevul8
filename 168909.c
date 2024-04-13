_dbus_auth_get_bytes_to_send (DBusAuth          *auth,
                              const DBusString **str)
{
  _dbus_assert (auth != NULL);
  _dbus_assert (str != NULL);

  *str = NULL;
  
  if (_dbus_string_get_length (&auth->outgoing) == 0)
    return FALSE;

  *str = &auth->outgoing;

  return TRUE;
}