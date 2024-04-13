process_ok(DBusAuth *auth,
          const DBusString *args_from_ok) {

  int end_of_hex;
  
  /* "args_from_ok" should be the GUID, whitespace already pulled off the front */
  _dbus_assert (_dbus_string_get_length (& DBUS_AUTH_CLIENT (auth)->guid_from_server) == 0);

  /* We decode the hex string to binary, using guid_from_server as scratch... */
  
  end_of_hex = 0;
  if (!_dbus_string_hex_decode (args_from_ok, 0, &end_of_hex,
                                & DBUS_AUTH_CLIENT (auth)->guid_from_server, 0))
    return FALSE;

  /* now clear out the scratch */
  _dbus_string_set_length (& DBUS_AUTH_CLIENT (auth)->guid_from_server, 0);
  
  if (end_of_hex != _dbus_string_get_length (args_from_ok) ||
      end_of_hex == 0)
    {
      _dbus_verbose ("Bad GUID from server, parsed %d bytes and had %d bytes from server\n",
                     end_of_hex, _dbus_string_get_length (args_from_ok));
      goto_state (auth, &common_state_need_disconnect);
      return TRUE;
    }

  if (!_dbus_string_copy (args_from_ok, 0, &DBUS_AUTH_CLIENT (auth)->guid_from_server, 0)) {
      _dbus_string_set_length (& DBUS_AUTH_CLIENT (auth)->guid_from_server, 0);
      return FALSE;
  }

  _dbus_verbose ("Got GUID '%s' from the server\n",
                 _dbus_string_get_const_data (& DBUS_AUTH_CLIENT (auth)->guid_from_server));

  if (auth->unix_fd_possible)
    return send_negotiate_unix_fd(auth);

  _dbus_verbose("Not negotiating unix fd passing, since not possible\n");
  return send_begin (auth);
}