handle_client_initial_response_external_mech (DBusAuth         *auth,
                                              DBusString       *response)
{
  /* We always append our UID as an initial response, so the server
   * doesn't have to send back an empty challenge to check whether we
   * want to specify an identity. i.e. this avoids a round trip that
   * the spec for the EXTERNAL mechanism otherwise requires.
   */
  DBusString plaintext;

  if (!_dbus_string_init (&plaintext))
    return FALSE;

  if (!_dbus_append_user_from_current_process (&plaintext))
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