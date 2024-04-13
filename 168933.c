_dbus_auth_encode_data (DBusAuth         *auth,
                        const DBusString *plaintext,
                        DBusString       *encoded)
{
  _dbus_assert (plaintext != encoded);
  
  if (auth->state != &common_state_authenticated)
    return FALSE;
  
  if (_dbus_auth_needs_encoding (auth))
    {
      if (DBUS_AUTH_IS_CLIENT (auth))
        return (* auth->mech->client_encode_func) (auth, plaintext, encoded);
      else
        return (* auth->mech->server_encode_func) (auth, plaintext, encoded);
    }
  else
    {
      return _dbus_string_copy (plaintext, 0, encoded,
                                _dbus_string_get_length (encoded));
    }
}