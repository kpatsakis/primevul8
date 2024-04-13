_dbus_auth_unref (DBusAuth *auth)
{
  _dbus_assert (auth != NULL);
  _dbus_assert (auth->refcount > 0);

  auth->refcount -= 1;
  if (auth->refcount == 0)
    {
      shutdown_mech (auth);

      if (DBUS_AUTH_IS_CLIENT (auth))
        {
          _dbus_string_free (& DBUS_AUTH_CLIENT (auth)->guid_from_server);
          _dbus_list_clear (& DBUS_AUTH_CLIENT (auth)->mechs_to_try);
        }
      else
        {
          _dbus_assert (DBUS_AUTH_IS_SERVER (auth));

          _dbus_string_free (& DBUS_AUTH_SERVER (auth)->guid);
        }

      if (auth->keyring)
        _dbus_keyring_unref (auth->keyring);

      _dbus_string_free (&auth->context);
      _dbus_string_free (&auth->challenge);
      _dbus_string_free (&auth->identity);
      _dbus_string_free (&auth->incoming);
      _dbus_string_free (&auth->outgoing);

      dbus_free_string_array (auth->allowed_mechs);

      _dbus_credentials_unref (auth->credentials);
      _dbus_credentials_unref (auth->authorized_identity);
      _dbus_credentials_unref (auth->desired_identity);
      
      dbus_free (auth);
    }
}