DEFUN (no_neighbor_peer_group,
       no_neighbor_peer_group_cmd,
       "no neighbor WORD peer-group",
       NO_STR
       NEIGHBOR_STR
       "Neighbor tag\n"
       "Configure peer-group\n")
{
  struct peer_group *group;

  group = peer_group_lookup (vty->index, argv[0]);
  if (group)
    peer_group_delete (group);
  else
    {
      vty_out (vty, "%% Create the peer-group first%s", VTY_NEWLINE);
      return CMD_WARNING;
    }
  return CMD_SUCCESS;
}