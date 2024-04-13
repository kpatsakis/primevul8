handle_client_initial_response_anonymous_mech (DBusAuth         *auth,
                                               DBusString       *response)
{
  /* Our initial response is a "trace" string which must be valid UTF-8
   * and must be an email address if it contains '@'.
   * We just send the dbus implementation info, like a user-agent or
   * something, because... why not. There's nothing guaranteed here
   * though, we could change it later.
   */
  DBusString plaintext;

  if (!_dbus_string_init (&plaintext))
    return FALSE;

  if (!_dbus_string_append (&plaintext,
                            "libdbus " DBUS_VERSION_STRING))
    goto failed;

  if (!_dbus_string_hex_encode (&plaintext, 0,
				response,
				_dbus_string_get_length (response)))
    goto failed;

  _dbus_string_free (&plaintext);
  
  return TRUE;

 failed:
  _dbus_string_free (&plaintext);
  return FALSE;  
}