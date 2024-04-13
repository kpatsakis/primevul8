_dbus_auth_return_buffer (DBusAuth               *auth,
                          DBusString             *buffer)
{
  _dbus_assert (buffer == &auth->incoming);
  _dbus_assert (auth->buffer_outstanding);

  auth->buffer_outstanding = FALSE;
}