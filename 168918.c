shutdown_mech (DBusAuth *auth)
{
  /* Cancel any auth */
  auth->already_asked_for_initial_response = FALSE;
  _dbus_string_set_length (&auth->identity, 0);

  _dbus_credentials_clear (auth->authorized_identity);
  _dbus_credentials_clear (auth->desired_identity);
  
  if (auth->mech != NULL)
    {
      _dbus_verbose ("%s: Shutting down mechanism %s\n",
                     DBUS_AUTH_NAME (auth), auth->mech->mechanism);
      
      if (DBUS_AUTH_IS_CLIENT (auth))
        (* auth->mech->client_shutdown_func) (auth);
      else
        (* auth->mech->server_shutdown_func) (auth);
      
      auth->mech = NULL;
    }
}