_dbus_auth_get_buffer (DBusAuth     *auth,
                       DBusString **buffer)
{
  _dbus_assert (auth != NULL);
  _dbus_assert (!auth->buffer_outstanding);
  
  *buffer = &auth->incoming;

  auth->buffer_outstanding = TRUE;
}