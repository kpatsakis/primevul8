zserv_command_string (unsigned int command)
{
  if (command >= sizeof(command_types)/sizeof(command_types[0]))
    {
      zlog_err ("unknown zserv command type: %u", command);
      return unknown.string;
    }
  return command_types[command].string;
}