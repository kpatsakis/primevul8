_dbus_auth_decode_data (DBusAuth         *auth,
                        const DBusString *encoded,
                        DBusString       *plaintext)
{
  _dbus_assert (plaintext != encoded);
  
  if (auth->state != &common_state_authenticated)
    return FALSE;
  
  if (_dbus_auth_needs_decoding (auth))
    {
      if (DBUS_AUTH_IS_CLIENT (auth))
        return (* auth->mech->client_decode_func) (auth, encoded, plaintext);
      else
        return (* auth->mech->server_decode_func) (auth, encoded, plaintext);
    }
  else
    {
      return _dbus_string_copy (encoded, 0, plaintext,
                                _dbus_string_get_length (plaintext));
    }
}