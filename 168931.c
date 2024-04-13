_dbus_auth_set_context (DBusAuth               *auth,
                        const DBusString       *context)
{
  return _dbus_string_replace_len (context, 0, _dbus_string_get_length (context),
                                   &auth->context, 0, _dbus_string_get_length (context));
}