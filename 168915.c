_dbus_auth_new (int size)
{
  DBusAuth *auth;
  
  auth = dbus_malloc0 (size);
  if (auth == NULL)
    return NULL;
  
  auth->refcount = 1;
  
  auth->keyring = NULL;
  auth->cookie_id = -1;
  
  /* note that we don't use the max string length feature,
   * because you can't use that feature if you're going to
   * try to recover from out-of-memory (it creates
   * what looks like unrecoverable inability to alloc
   * more space in the string). But we do handle
   * overlong buffers in _dbus_auth_do_work().
   */
  
  if (!_dbus_string_init (&auth->incoming))
    goto enomem_0;

  if (!_dbus_string_init (&auth->outgoing))
    goto enomem_1;
    
  if (!_dbus_string_init (&auth->identity))
    goto enomem_2;

  if (!_dbus_string_init (&auth->context))
    goto enomem_3;

  if (!_dbus_string_init (&auth->challenge))
    goto enomem_4;

  /* default context if none is specified */
  if (!_dbus_string_append (&auth->context, "org_freedesktop_general"))
    goto enomem_5;

  auth->credentials = _dbus_credentials_new ();
  if (auth->credentials == NULL)
    goto enomem_6;
  
  auth->authorized_identity = _dbus_credentials_new ();
  if (auth->authorized_identity == NULL)
    goto enomem_7;

  auth->desired_identity = _dbus_credentials_new ();
  if (auth->desired_identity == NULL)
    goto enomem_8;
  
  return auth;

#if 0
 enomem_9:
  _dbus_credentials_unref (auth->desired_identity);
#endif
 enomem_8:
  _dbus_credentials_unref (auth->authorized_identity);
 enomem_7:
  _dbus_credentials_unref (auth->credentials);
 enomem_6:
 /* last alloc was an append to context, which is freed already below */ ;
 enomem_5:
  _dbus_string_free (&auth->challenge);
 enomem_4:
  _dbus_string_free (&auth->context);
 enomem_3:
  _dbus_string_free (&auth->identity);
 enomem_2:
  _dbus_string_free (&auth->outgoing);
 enomem_1:
  _dbus_string_free (&auth->incoming);
 enomem_0:
  dbus_free (auth);
  return NULL;
}