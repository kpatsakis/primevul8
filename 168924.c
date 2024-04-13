handle_client_initial_response_cookie_sha1_mech (DBusAuth   *auth,
                                                 DBusString *response)
{
  DBusString username;
  dbus_bool_t retval;

  retval = FALSE;

  if (!_dbus_string_init (&username))
    return FALSE;
  
  if (!_dbus_append_user_from_current_process (&username))
    goto out_0;

  if (!_dbus_string_hex_encode (&username, 0,
				response,
				_dbus_string_get_length (response)))
    goto out_0;

  retval = TRUE;
  
 out_0:
  _dbus_string_free (&username);
  
  return retval;
}