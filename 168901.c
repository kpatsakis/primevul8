handle_auth (DBusAuth *auth, const DBusString *args)
{
  if (_dbus_string_get_length (args) == 0)
    {
      /* No args to the auth, send mechanisms */
      if (!send_rejected (auth))
        return FALSE;

      return TRUE;
    }
  else
    {
      int i;
      DBusString mech;
      DBusString hex_response;
      
      _dbus_string_find_blank (args, 0, &i);

      if (!_dbus_string_init (&mech))
        return FALSE;

      if (!_dbus_string_init (&hex_response))
        {
          _dbus_string_free (&mech);
          return FALSE;
        }
      
      if (!_dbus_string_copy_len (args, 0, i, &mech, 0))
        goto failed;

      _dbus_string_skip_blank (args, i, &i);
      if (!_dbus_string_copy (args, i, &hex_response, 0))
        goto failed;
     
      auth->mech = find_mech (&mech, auth->allowed_mechs);
      if (auth->mech != NULL)
        {
          _dbus_verbose ("%s: Trying mechanism %s\n",
                         DBUS_AUTH_NAME (auth),
                         auth->mech->mechanism);
          
          if (!process_data (auth, &hex_response,
                             auth->mech->server_data_func))
            goto failed;
        }
      else
        {
          /* Unsupported mechanism */
          _dbus_verbose ("%s: Unsupported mechanism %s\n",
                         DBUS_AUTH_NAME (auth),
                         _dbus_string_get_const_data (&mech));
          
          if (!send_rejected (auth))
            goto failed;
        }

      _dbus_string_free (&mech);      
      _dbus_string_free (&hex_response);

      return TRUE;
      
    failed:
      auth->mech = NULL;
      _dbus_string_free (&mech);
      _dbus_string_free (&hex_response);
      return FALSE;
    }
}