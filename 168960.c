_dbus_auth_get_unused_bytes (DBusAuth           *auth,
                             const DBusString **str)
{
  if (!DBUS_AUTH_IN_END_STATE (auth))
    return;

  *str = &auth->incoming;
}