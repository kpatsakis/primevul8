_dbus_auth_client_new (void)
{
  DBusAuth *auth;
  DBusString guid_str;

  if (!_dbus_string_init (&guid_str))
    return NULL;

  auth = _dbus_auth_new (sizeof (DBusAuthClient));
  if (auth == NULL)
    {
      _dbus_string_free (&guid_str);
      return NULL;
    }

  DBUS_AUTH_CLIENT (auth)->guid_from_server = guid_str;

  auth->side = auth_side_client;
  auth->state = &client_state_need_send_auth;

  /* Start the auth conversation by sending AUTH for our default
   * mechanism */
  if (!send_auth (auth, &all_mechanisms[0]))
    {
      _dbus_auth_unref (auth);
      return NULL;
    }
  
  return auth;
}