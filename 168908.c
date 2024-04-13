send_rejected (DBusAuth *auth)
{
  DBusString command;
  DBusAuthServer *server_auth;
  int i;
  
  if (!_dbus_string_init (&command))
    return FALSE;
  
  if (!_dbus_string_append (&command,
                            "REJECTED"))
    goto nomem;

  for (i = 0; all_mechanisms[i].mechanism != NULL; i++)
    {
      /* skip mechanisms that aren't allowed */
      if (auth->allowed_mechs != NULL &&
          !_dbus_string_array_contains ((const char**)auth->allowed_mechs,
                                        all_mechanisms[i].mechanism))
        continue;

      if (!_dbus_string_append (&command,
                                " "))
        goto nomem;

      if (!_dbus_string_append (&command,
                                all_mechanisms[i].mechanism))
        goto nomem;
    }
  
  if (!_dbus_string_append (&command, "\r\n"))
    goto nomem;

  if (!_dbus_string_copy (&command, 0, &auth->outgoing,
                          _dbus_string_get_length (&auth->outgoing)))
    goto nomem;

  shutdown_mech (auth);
  
  _dbus_assert (DBUS_AUTH_IS_SERVER (auth));
  server_auth = DBUS_AUTH_SERVER (auth);
  server_auth->failures += 1;

  if (server_auth->failures >= server_auth->max_failures)
    goto_state (auth, &common_state_need_disconnect);
  else
    goto_state (auth, &server_state_waiting_for_auth);

  _dbus_string_free (&command);
  
  return TRUE;

 nomem:
  _dbus_string_free (&command);
  return FALSE;
}