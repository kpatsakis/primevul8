process_command (DBusAuth *auth)
{
  DBusAuthCommand command;
  DBusString line;
  DBusString args;
  int eol;
  int i, j;
  dbus_bool_t retval;

  /* _dbus_verbose ("%s:   trying process_command()\n"); */
  
  retval = FALSE;
  
  eol = 0;
  if (!_dbus_string_find (&auth->incoming, 0, "\r\n", &eol))
    return FALSE;
  
  if (!_dbus_string_init (&line))
    {
      auth->needed_memory = TRUE;
      return FALSE;
    }

  if (!_dbus_string_init (&args))
    {
      _dbus_string_free (&line);
      auth->needed_memory = TRUE;
      return FALSE;
    }
  
  if (!_dbus_string_copy_len (&auth->incoming, 0, eol, &line, 0))
    goto out;

  if (!_dbus_string_validate_ascii (&line, 0,
                                    _dbus_string_get_length (&line)))
    {
      _dbus_verbose ("%s: Command contained non-ASCII chars or embedded nul\n",
                     DBUS_AUTH_NAME (auth));
      if (!send_error (auth, "Command contained non-ASCII"))
        goto out;
      else
        goto next_command;
    }
  
  _dbus_verbose ("%s: got command \"%s\"\n",
                 DBUS_AUTH_NAME (auth),
                 _dbus_string_get_const_data (&line));
  
  _dbus_string_find_blank (&line, 0, &i);
  _dbus_string_skip_blank (&line, i, &j);

  if (j > i)
    _dbus_string_delete (&line, i, j - i);
  
  if (!_dbus_string_move (&line, i, &args, 0))
    goto out;

  /* FIXME 1.0 we should probably validate that only the allowed
   * chars are in the command name
   */
  
  command = lookup_command_from_name (&line);
  if (!(* auth->state->handler) (auth, command, &args))
    goto out;

 next_command:
  
  /* We've succeeded in processing the whole command so drop it out
   * of the incoming buffer and return TRUE to try another command.
   */

  _dbus_string_delete (&auth->incoming, 0, eol);
  
  /* kill the \r\n */
  _dbus_string_delete (&auth->incoming, 0, 2);

  retval = TRUE;
  
 out:
  _dbus_string_free (&args);
  _dbus_string_free (&line);

  if (!retval)
    auth->needed_memory = TRUE;
  else
    auth->needed_memory = FALSE;
  
  return retval;
}