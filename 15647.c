DEFUN (no_neighbor_peer_group_remote_as,
       no_neighbor_peer_group_remote_as_cmd,
       "no neighbor WORD remote-as <1-65535>",
       NO_STR
       NEIGHBOR_STR
       "Neighbor tag\n"
       "Specify a BGP neighbor\n"
       AS_STR)
{
  struct peer_group *group;

  group = peer_group_lookup (vty->index, argv[0]);
  if (group)
    peer_group_remote_as_delete (group);
  else
    {
      vty_out (vty, "%% Create the peer-group first%s", VTY_NEWLINE);
      return CMD_WARNING;
    }
  return CMD_SUCCESS;
}