handle_server_data_external_mech (DBusAuth         *auth,
                                  const DBusString *data)
{
  if (_dbus_credentials_are_anonymous (auth->credentials))
    {
      _dbus_verbose ("%s: no credentials, mechanism EXTERNAL can't authenticate\n",
                     DBUS_AUTH_NAME (auth));
      return send_rejected (auth);
    }
  
  if (_dbus_string_get_length (data) > 0)
    {
      if (_dbus_string_get_length (&auth->identity) > 0)
        {
          /* Tried to send two auth identities, wtf */
          _dbus_verbose ("%s: client tried to send auth identity, but we already have one\n",
                         DBUS_AUTH_NAME (auth));
          return send_rejected (auth);
        }
      else
        {
          /* this is our auth identity */
          if (!_dbus_string_copy (data, 0, &auth->identity, 0))
            return FALSE;
        }
    }

  /* Poke client for an auth identity, if none given */
  if (_dbus_string_get_length (&auth->identity) == 0 &&
      !auth->already_asked_for_initial_response)
    {
      if (send_data (auth, NULL))
        {
          _dbus_verbose ("%s: sending empty challenge asking client for auth identity\n",
                         DBUS_AUTH_NAME (auth));
          auth->already_asked_for_initial_response = TRUE;
          goto_state (auth, &server_state_waiting_for_data);
          return TRUE;
        }
      else
        return FALSE;
    }

  _dbus_credentials_clear (auth->desired_identity);
  
  /* If auth->identity is still empty here, then client
   * responded with an empty string after we poked it for
   * an initial response. This means to try to auth the
   * identity provided in the credentials.
   */
  if (_dbus_string_get_length (&auth->identity) == 0)
    {
      if (!_dbus_credentials_add_credentials (auth->desired_identity,
                                              auth->credentials))
        {
          return FALSE; /* OOM */
        }
    }
  else
    {
      if (!_dbus_credentials_add_from_user (auth->desired_identity,
                                            &auth->identity))
        {
          _dbus_verbose ("%s: could not get credentials from uid string\n",
                         DBUS_AUTH_NAME (auth));
          return send_rejected (auth);
        }
    }

  if (_dbus_credentials_are_anonymous (auth->desired_identity))
    {
      _dbus_verbose ("%s: desired user %s is no good\n",
                     DBUS_AUTH_NAME (auth),
                     _dbus_string_get_const_data (&auth->identity));
      return send_rejected (auth);
    }
  
  if (_dbus_credentials_are_superset (auth->credentials,
                                      auth->desired_identity))
    {
      /* client has authenticated */
      if (!_dbus_credentials_add_credentials (auth->authorized_identity,
                                              auth->desired_identity))
        return FALSE;

      /* also copy misc process info from the socket credentials
       */
      if (!_dbus_credentials_add_credential (auth->authorized_identity,
                                             DBUS_CREDENTIAL_UNIX_PROCESS_ID,
                                             auth->credentials))
        return FALSE;

      if (!_dbus_credentials_add_credential (auth->authorized_identity,
                                             DBUS_CREDENTIAL_ADT_AUDIT_DATA_ID,
                                             auth->credentials))
        return FALSE;

      if (!_dbus_credentials_add_credential (auth->authorized_identity,
                                             DBUS_CREDENTIAL_LINUX_SECURITY_LABEL,
                                             auth->credentials))
        return FALSE;

      if (!send_ok (auth))
        return FALSE;

      _dbus_verbose ("%s: authenticated client based on socket credentials\n",
                     DBUS_AUTH_NAME (auth));

      return TRUE;
    }
  else
    {
      _dbus_verbose ("%s: desired identity not found in socket credentials\n",
                     DBUS_AUTH_NAME (auth));
      return send_rejected (auth);
    }
}