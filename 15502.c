DEFUN (neighbor_description,
       neighbor_description_cmd,
       NEIGHBOR_CMD2 "description .LINE",
       NEIGHBOR_STR
       NEIGHBOR_ADDR_STR2
       "Neighbor specific description\n"
       "Up to 80 characters describing this neighbor\n")
{
  struct peer *peer;
  char *str;

  peer = peer_and_group_lookup_vty (vty, argv[0]);
  if (! peer)
    return CMD_WARNING;

  if (argc == 1)
    return CMD_SUCCESS;

  str = argv_concat(argv, argc, 1);

  peer_description_set (peer, str);

  XFREE (MTYPE_TMP, str);

  return CMD_SUCCESS;
}