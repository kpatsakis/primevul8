send_cancel (DBusAuth *auth)
{
  if (_dbus_string_append (&auth->outgoing, "CANCEL\r\n"))
    {
      goto_state (auth, &client_state_waiting_for_reject);
      return TRUE;
    }
  else
    return FALSE;
}