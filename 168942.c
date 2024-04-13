send_auth (DBusAuth *auth, const DBusAuthMechanismHandler *mech)
{
  DBusString auth_command;

  if (!_dbus_string_init (&auth_command))
    return FALSE;
      
  if (!_dbus_string_append (&auth_command,
                            "AUTH "))
    {
      _dbus_string_free (&auth_command);
      return FALSE;
    }  
  
  if (!_dbus_string_append (&auth_command,
                            mech->mechanism))
    {
      _dbus_string_free (&auth_command);
      return FALSE;
    }

  if (mech->client_initial_response_func != NULL)
    {
      if (!_dbus_string_append (&auth_command, " "))
        {
          _dbus_string_free (&auth_command);
          return FALSE;
        }
      
      if (!(* mech->client_initial_response_func) (auth, &auth_command))
        {
          _dbus_string_free (&auth_command);
          return FALSE;
        }
    }
  
  if (!_dbus_string_append (&auth_command,
                            "\r\n"))
    {
      _dbus_string_free (&auth_command);
      return FALSE;
    }

  if (!_dbus_string_copy (&auth_command, 0,
                          &auth->outgoing,
                          _dbus_string_get_length (&auth->outgoing)))
    {
      _dbus_string_free (&auth_command);
      return FALSE;
    }

  _dbus_string_free (&auth_command);
  shutdown_mech (auth);
  auth->mech = mech;      
  goto_state (auth, &client_state_waiting_for_data);

  return TRUE;
}