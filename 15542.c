DEFUN (no_neighbor,
       no_neighbor_cmd,
       NO_NEIGHBOR_CMD2,
       NO_STR
       NEIGHBOR_STR
       NEIGHBOR_ADDR_STR2)
{
  int ret;
  union sockunion su;
  struct peer_group *group;
  struct peer *peer;

  ret = str2sockunion (argv[0], &su);
  if (ret < 0)
    {
      group = peer_group_lookup (vty->index, argv[0]);
      if (group)
	peer_group_delete (group);
      else
	{
	  vty_out (vty, "%% Create the peer-group first%s", VTY_NEWLINE);
	  return CMD_WARNING;
	}
    }
  else
    {
      peer = peer_lookup (vty->index, &su);
      if (peer)
        peer_delete (peer);
    }

  return CMD_SUCCESS;
}