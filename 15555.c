DEFUN (neighbor_peer_group,
       neighbor_peer_group_cmd,
       "neighbor WORD peer-group",
       NEIGHBOR_STR
       "Neighbor tag\n"
       "Configure peer-group\n")
{
  struct bgp *bgp;
  struct peer_group *group;

  bgp = vty->index;

  group = peer_group_get (bgp, argv[0]);
  if (! group)
    return CMD_WARNING;

  return CMD_SUCCESS;
}