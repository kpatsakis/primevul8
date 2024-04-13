_dbus_auth_set_credentials (DBusAuth               *auth,
                            DBusCredentials        *credentials)
{
  _dbus_credentials_clear (auth->credentials);
  return _dbus_credentials_add_credentials (auth->credentials,
                                            credentials);
}