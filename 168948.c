_dbus_auth_get_guid_from_server (DBusAuth *auth)
{
  _dbus_assert (DBUS_AUTH_IS_CLIENT (auth));
  
  if (auth->state == &common_state_authenticated)
    return _dbus_string_get_const_data (& DBUS_AUTH_CLIENT (auth)->guid_from_server);
  else
    return NULL;
}