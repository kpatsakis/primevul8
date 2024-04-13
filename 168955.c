_dbus_auth_server_new (const DBusString *guid)
{
  DBusAuth *auth;
  DBusAuthServer *server_auth;
  DBusString guid_copy;

  if (!_dbus_string_init (&guid_copy))
    return NULL;

  if (!_dbus_string_copy (guid, 0, &guid_copy, 0))
    {
      _dbus_string_free (&guid_copy);
      return NULL;
    }

  auth = _dbus_auth_new (sizeof (DBusAuthServer));
  if (auth == NULL)
    {
      _dbus_string_free (&guid_copy);
      return NULL;
    }
  
  auth->side = auth_side_server;
  auth->state = &server_state_waiting_for_auth;

  server_auth = DBUS_AUTH_SERVER (auth);

  server_auth->guid = guid_copy;
  
  /* perhaps this should be per-mechanism with a lower
   * max
   */
  server_auth->failures = 0;
  server_auth->max_failures = 6;
  
  return auth;
}