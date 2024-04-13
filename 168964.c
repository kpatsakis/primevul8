_dbus_auth_delete_unused_bytes (DBusAuth *auth)
{
  if (!DBUS_AUTH_IN_END_STATE (auth))
    return;

  _dbus_string_set_length (&auth->incoming, 0);
}