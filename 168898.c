sha1_handle_second_client_response (DBusAuth         *auth,
                                    const DBusString *data)
{
  /* We are expecting a response which is the hex-encoded client
   * challenge, space, then SHA-1 hash of the concatenation of our
   * challenge, ":", client challenge, ":", secret key, all
   * hex-encoded.
   */
  int i;
  DBusString client_challenge;
  DBusString client_hash;
  dbus_bool_t retval;
  DBusString correct_hash;
  
  retval = FALSE;
  
  if (!_dbus_string_find_blank (data, 0, &i))
    {
      _dbus_verbose ("%s: no space separator in client response\n",
                     DBUS_AUTH_NAME (auth));
      return send_rejected (auth);
    }
  
  if (!_dbus_string_init (&client_challenge))
    goto out_0;

  if (!_dbus_string_init (&client_hash))
    goto out_1;  

  if (!_dbus_string_copy_len (data, 0, i, &client_challenge,
                              0))
    goto out_2;

  _dbus_string_skip_blank (data, i, &i);
  
  if (!_dbus_string_copy_len (data, i,
                              _dbus_string_get_length (data) - i,
                              &client_hash,
                              0))
    goto out_2;

  if (_dbus_string_get_length (&client_challenge) == 0 ||
      _dbus_string_get_length (&client_hash) == 0)
    {
      _dbus_verbose ("%s: zero-length client challenge or hash\n",
                     DBUS_AUTH_NAME (auth));
      if (send_rejected (auth))
        retval = TRUE;
      goto out_2;
    }

  if (!_dbus_string_init (&correct_hash))
    goto out_2;

  if (!sha1_compute_hash (auth, auth->cookie_id,
                          &auth->challenge, 
                          &client_challenge,
                          &correct_hash))
    goto out_3;

  /* if cookie_id was invalid, then we get an empty hash */
  if (_dbus_string_get_length (&correct_hash) == 0)
    {
      if (send_rejected (auth))
        retval = TRUE;
      goto out_3;
    }
  
  if (!_dbus_string_equal (&client_hash, &correct_hash))
    {
      if (send_rejected (auth))
        retval = TRUE;
      goto out_3;
    }

  if (!_dbus_credentials_add_credentials (auth->authorized_identity,
                                          auth->desired_identity))
    goto out_3;

  /* Copy process ID from the socket credentials if it's there
   */
  if (!_dbus_credentials_add_credential (auth->authorized_identity,
                                         DBUS_CREDENTIAL_UNIX_PROCESS_ID,
                                         auth->credentials))
    goto out_3;
  
  if (!send_ok (auth))
    goto out_3;

  _dbus_verbose ("%s: authenticated client using DBUS_COOKIE_SHA1\n",
                 DBUS_AUTH_NAME (auth));
  
  retval = TRUE;
  
 out_3:
  _dbus_string_zero (&correct_hash);
  _dbus_string_free (&correct_hash);
 out_2:
  _dbus_string_zero (&client_hash);
  _dbus_string_free (&client_hash);
 out_1:
  _dbus_string_free (&client_challenge);
 out_0:
  return retval;
}