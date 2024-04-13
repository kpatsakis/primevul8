_dbus_auth_do_work (DBusAuth *auth)
{
  auth->needed_memory = FALSE;

  /* Max amount we'll buffer up before deciding someone's on crack */
#define MAX_BUFFER (16 * _DBUS_ONE_KILOBYTE)

  do
    {
      if (DBUS_AUTH_IN_END_STATE (auth))
        break;
      
      if (_dbus_string_get_length (&auth->incoming) > MAX_BUFFER ||
          _dbus_string_get_length (&auth->outgoing) > MAX_BUFFER)
        {
          goto_state (auth, &common_state_need_disconnect);
          _dbus_verbose ("%s: Disconnecting due to excessive data buffered in auth phase\n",
                         DBUS_AUTH_NAME (auth));
          break;
        }
    }
  while (process_command (auth));

  if (auth->needed_memory)
    return DBUS_AUTH_STATE_WAITING_FOR_MEMORY;
  else if (_dbus_string_get_length (&auth->outgoing) > 0)
    return DBUS_AUTH_STATE_HAVE_BYTES_TO_SEND;
  else if (auth->state == &common_state_need_disconnect)
    return DBUS_AUTH_STATE_NEED_DISCONNECT;
  else if (auth->state == &common_state_authenticated)
    return DBUS_AUTH_STATE_AUTHENTICATED;
  else return DBUS_AUTH_STATE_WAITING_FOR_INPUT;
}