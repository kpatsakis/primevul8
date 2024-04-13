record_mechanisms (DBusAuth         *auth,
                   const DBusString *args)
{
  int next;
  int len;

  if (auth->already_got_mechanisms)
    return TRUE;
  
  len = _dbus_string_get_length (args);
  
  next = 0;
  while (next < len)
    {
      DBusString m;
      const DBusAuthMechanismHandler *mech;
      
      if (!_dbus_string_init (&m))
        goto nomem;
      
      if (!get_word (args, &next, &m))
        {
          _dbus_string_free (&m);
          goto nomem;
        }

      mech = find_mech (&m, auth->allowed_mechs);

      if (mech != NULL)
        {
          /* FIXME right now we try mechanisms in the order
           * the server lists them; should we do them in
           * some more deterministic order?
           *
           * Probably in all_mechanisms order, our order of
           * preference. Of course when the server is us,
           * it lists things in that order anyhow.
           */

          if (mech != &all_mechanisms[0])
            {
              _dbus_verbose ("%s: Adding mechanism %s to list we will try\n",
                             DBUS_AUTH_NAME (auth), mech->mechanism);
          
              if (!_dbus_list_append (& DBUS_AUTH_CLIENT (auth)->mechs_to_try,
                                      (void*) mech))
                {
                  _dbus_string_free (&m);
                  goto nomem;
                }
            }
          else
            {
              _dbus_verbose ("%s: Already tried mechanism %s; not adding to list we will try\n",
                             DBUS_AUTH_NAME (auth), mech->mechanism);
            }
        }
      else
        {
          _dbus_verbose ("%s: Server offered mechanism \"%s\" that we don't know how to use\n",
                         DBUS_AUTH_NAME (auth),
                         _dbus_string_get_const_data (&m));
        }

      _dbus_string_free (&m);
    }
  
  auth->already_got_mechanisms = TRUE;
  
  return TRUE;

 nomem:
  _dbus_list_clear (& DBUS_AUTH_CLIENT (auth)->mechs_to_try);
  
  return FALSE;
}