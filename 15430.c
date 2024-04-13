community_list_unset_vty (struct vty *vty, int argc, const char **argv,
			  int style)
{
  int ret;
  int direct = 0;
  char *str = NULL;

  if (argc > 1)
    {
      /* Check the list direct. */
      if (strncmp (argv[1], "p", 1) == 0)
	direct = COMMUNITY_PERMIT;
      else if (strncmp (argv[1], "d", 1) == 0)
	direct = COMMUNITY_DENY;
      else
	{
	  vty_out (vty, "%% Matching condition must be permit or deny%s",
		   VTY_NEWLINE);
	  return CMD_WARNING;
	}

      /* Concat community string argument.  */
      str = argv_concat (argv, argc, 2);
    }

  /* Unset community list.  */
  ret = community_list_unset (bgp_clist, argv[0], str, direct, style);

  /* Free temporary community list string allocated by
     argv_concat().  */
  if (str)
    XFREE (MTYPE_TMP, str);

  if (ret < 0)
    {
      community_list_perror (vty, ret);
      return CMD_WARNING;
    }

  return CMD_SUCCESS;
}