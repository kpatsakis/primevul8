handle_server_data_anonymous_mech (DBusAuth         *auth,
                                   const DBusString *data)
{  
  if (_dbus_string_get_length (data) > 0)
    {
      /* Client is allowed to send "trace" data, the only defined
       * meaning is that if it contains '@' it is an email address,
       * and otherwise it is anything else, and it's supposed to be
       * UTF-8
       */
      if (!_dbus_string_validate_utf8 (data, 0, _dbus_string_get_length (data)))
        {
          _dbus_verbose ("%s: Received invalid UTF-8 trace data from ANONYMOUS client\n",
                         DBUS_AUTH_NAME (auth));
          return send_rejected (auth);
        }
      
      _dbus_verbose ("%s: ANONYMOUS client sent trace string: '%s'\n",
                     DBUS_AUTH_NAME (auth),
                     _dbus_string_get_const_data (data));
    }

  /* We want to be anonymous (clear in case some other protocol got midway through I guess) */
  _dbus_credentials_clear (auth->desired_identity);

  /* Copy process ID from the socket credentials
   */
  if (!_dbus_credentials_add_credential (auth->authorized_identity,
                                         DBUS_CREDENTIAL_UNIX_PROCESS_ID,
                                         auth->credentials))
    return FALSE;
  
  /* Anonymous is always allowed */
  if (!send_ok (auth))
    return FALSE;

  _dbus_verbose ("%s: authenticated client as anonymous\n",
                 DBUS_AUTH_NAME (auth));

  return TRUE;
}