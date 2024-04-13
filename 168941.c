_dbus_auth_get_identity (DBusAuth               *auth)
{
  if (auth->state == &common_state_authenticated)
    {
      return auth->authorized_identity;
    }
  else
    {
      /* FIXME instead of this, keep an empty credential around that
       * doesn't require allocation or something
       */
      /* return empty credentials */
      _dbus_assert (_dbus_credentials_are_empty (auth->authorized_identity));
      return auth->authorized_identity;
    }
}