_dbus_auth_is_supported_mechanism (DBusString *name)
{
  _dbus_assert (name != NULL);

  return find_mech (name, NULL) != NULL;
}