_dbus_auth_set_unix_fd_possible(DBusAuth *auth, dbus_bool_t b)
{
  auth->unix_fd_possible = b;
}