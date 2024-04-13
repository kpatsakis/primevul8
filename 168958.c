sha1_compute_hash (DBusAuth         *auth,
                   int               cookie_id,
                   const DBusString *server_challenge,
                   const DBusString *client_challenge,
                   DBusString       *hash)
{
  DBusString cookie;
  DBusString to_hash;
  dbus_bool_t retval;
  
  _dbus_assert (auth->keyring != NULL);

  retval = FALSE;
  
  if (!_dbus_string_init (&cookie))
    return FALSE;

  if (!_dbus_keyring_get_hex_key (auth->keyring, cookie_id,
                                  &cookie))
    goto out_0;

  if (_dbus_string_get_length (&cookie) == 0)
    {
      retval = TRUE;
      goto out_0;
    }

  if (!_dbus_string_init (&to_hash))
    goto out_0;
  
  if (!_dbus_string_copy (server_challenge, 0,
                          &to_hash, _dbus_string_get_length (&to_hash)))
    goto out_1;

  if (!_dbus_string_append (&to_hash, ":"))
    goto out_1;
  
  if (!_dbus_string_copy (client_challenge, 0,
                          &to_hash, _dbus_string_get_length (&to_hash)))
    goto out_1;

  if (!_dbus_string_append (&to_hash, ":"))
    goto out_1;

  if (!_dbus_string_copy (&cookie, 0,
                          &to_hash, _dbus_string_get_length (&to_hash)))
    goto out_1;

  if (!_dbus_sha_compute (&to_hash, hash))
    goto out_1;
  
  retval = TRUE;

 out_1:
  _dbus_string_zero (&to_hash);
  _dbus_string_free (&to_hash);
 out_0:
  _dbus_string_zero (&cookie);
  _dbus_string_free (&cookie);
  return retval;
}