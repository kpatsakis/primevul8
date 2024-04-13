process_rejected (DBusAuth *auth, const DBusString *args)
{
  const DBusAuthMechanismHandler *mech;
  DBusAuthClient *client;

  client = DBUS_AUTH_CLIENT (auth);

  if (!auth->already_got_mechanisms)
    {
      if (!record_mechanisms (auth, args))
        return FALSE;
    }
  
  if (DBUS_AUTH_CLIENT (auth)->mechs_to_try != NULL)
    {
      mech = client->mechs_to_try->data;

      if (!send_auth (auth, mech))
        return FALSE;

      _dbus_list_pop_first (&client->mechs_to_try);

      _dbus_verbose ("%s: Trying mechanism %s\n",
                     DBUS_AUTH_NAME (auth),
                     mech->mechanism);
    }
  else
    {
      /* Give up */
      _dbus_verbose ("%s: Disconnecting because we are out of mechanisms to try using\n",
                     DBUS_AUTH_NAME (auth));
      goto_state (auth, &common_state_need_disconnect);
    }
  
  return TRUE;
}