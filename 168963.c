handle_client_data_cookie_sha1_mech (DBusAuth         *auth,
                                     const DBusString *data)
{
  /* The data we get from the server should be the cookie context
   * name, the cookie ID, and the server challenge, separated by
   * spaces. We send back our challenge string and the correct hash.
   */
  dbus_bool_t retval = FALSE;
  DBusString context;
  DBusString cookie_id_str;
  DBusString server_challenge;
  DBusString client_challenge;
  DBusString correct_hash;
  DBusString tmp;
  int i, j;
  long val;
  DBusError error = DBUS_ERROR_INIT;

  if (!_dbus_string_find_blank (data, 0, &i))
    {
      if (send_error (auth,
                      "Server did not send context/ID/challenge properly"))
        retval = TRUE;
      goto out_0;
    }

  if (!_dbus_string_init (&context))
    goto out_0;

  if (!_dbus_string_copy_len (data, 0, i,
                              &context, 0))
    goto out_1;
  
  _dbus_string_skip_blank (data, i, &i);
  if (!_dbus_string_find_blank (data, i, &j))
    {
      if (send_error (auth,
                      "Server did not send context/ID/challenge properly"))
        retval = TRUE;
      goto out_1;
    }

  if (!_dbus_string_init (&cookie_id_str))
    goto out_1;
  
  if (!_dbus_string_copy_len (data, i, j - i,
                              &cookie_id_str, 0))
    goto out_2;  

  if (!_dbus_string_init (&server_challenge))
    goto out_2;

  i = j;
  _dbus_string_skip_blank (data, i, &i);
  j = _dbus_string_get_length (data);

  if (!_dbus_string_copy_len (data, i, j - i,
                              &server_challenge, 0))
    goto out_3;

  if (!_dbus_keyring_validate_context (&context))
    {
      if (send_error (auth, "Server sent invalid cookie context"))
        retval = TRUE;
      goto out_3;
    }

  if (!_dbus_string_parse_int (&cookie_id_str, 0, &val, NULL))
    {
      if (send_error (auth, "Could not parse cookie ID as an integer"))
        retval = TRUE;
      goto out_3;
    }

  if (_dbus_string_get_length (&server_challenge) == 0)
    {
      if (send_error (auth, "Empty server challenge string"))
        retval = TRUE;
      goto out_3;
    }

  if (auth->keyring == NULL)
    {
      auth->keyring = _dbus_keyring_new_for_credentials (NULL,
                                                         &context,
                                                         &error);

      if (auth->keyring == NULL)
        {
          if (dbus_error_has_name (&error,
                                   DBUS_ERROR_NO_MEMORY))
            {
              dbus_error_free (&error);
              goto out_3;
            }
          else
            {
              _DBUS_ASSERT_ERROR_IS_SET (&error);

              _dbus_verbose ("%s: Error loading keyring: %s\n",
                             DBUS_AUTH_NAME (auth), error.message);
              
              if (send_error (auth, "Could not load cookie file"))
                retval = TRUE; /* retval is only about mem */
              
              dbus_error_free (&error);
              goto out_3;
            }
        }
      else
        {
          _dbus_assert (!dbus_error_is_set (&error));
        }
    }
  
  _dbus_assert (auth->keyring != NULL);
  
  if (!_dbus_string_init (&tmp))
    goto out_3;

  if (!_dbus_generate_random_bytes (&tmp, N_CHALLENGE_BYTES, &error))
    {
      if (dbus_error_has_name (&error, DBUS_ERROR_NO_MEMORY))
        {
          dbus_error_free (&error);
          goto out_4;
        }
      else
        {
          _DBUS_ASSERT_ERROR_IS_SET (&error);

          _dbus_verbose ("%s: Failed to generate challenge: %s\n",
                         DBUS_AUTH_NAME (auth), error.message);

          if (send_error (auth, "Failed to generate challenge"))
            retval = TRUE; /* retval is only about mem */

          dbus_error_free (&error);
          goto out_4;
        }
    }

  if (!_dbus_string_init (&client_challenge))
    goto out_4;

  if (!_dbus_string_hex_encode (&tmp, 0, &client_challenge, 0))
    goto out_5;

  if (!_dbus_string_init (&correct_hash))
    goto out_5;
  
  if (!sha1_compute_hash (auth, val,
                          &server_challenge,
                          &client_challenge,
                          &correct_hash))
    goto out_6;

  if (_dbus_string_get_length (&correct_hash) == 0)
    {
      /* couldn't find the cookie ID or something */
      if (send_error (auth, "Don't have the requested cookie ID"))
        retval = TRUE;
      goto out_6;
    }
  
  _dbus_string_set_length (&tmp, 0);
  
  if (!_dbus_string_copy (&client_challenge, 0, &tmp,
                          _dbus_string_get_length (&tmp)))
    goto out_6;

  if (!_dbus_string_append (&tmp, " "))
    goto out_6;

  if (!_dbus_string_copy (&correct_hash, 0, &tmp,
                          _dbus_string_get_length (&tmp)))
    goto out_6;

  if (!send_data (auth, &tmp))
    goto out_6;

  retval = TRUE;

 out_6:
  _dbus_string_zero (&correct_hash);
  _dbus_string_free (&correct_hash);
 out_5:
  _dbus_string_free (&client_challenge);
 out_4:
  _dbus_string_zero (&tmp);
  _dbus_string_free (&tmp);
 out_3:
  _dbus_string_free (&server_challenge);
 out_2:
  _dbus_string_free (&cookie_id_str);
 out_1:
  _dbus_string_free (&context);
 out_0:
  return retval;
}