_dbus_auth_ref (DBusAuth *auth)
{
  _dbus_assert (auth != NULL);
  
  auth->refcount += 1;
  
  return auth;
}