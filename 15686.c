community_list_set_vty (struct vty *vty, int argc, const char **argv, 
                        int style, int reject_all_digit_name)
{
  int ret;
  int direct;
  char *str;

  /* Check the list type. */
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

  /* All digit name check.  */
  if (reject_all_digit_name && all_digit (argv[0]))
    {
      vty_out (vty, "%% Community name cannot have all digits%s", VTY_NEWLINE);
      return CMD_WARNING;
    }

  /* Concat community string argument.  */
  if (argc > 1)
    str = argv_concat (argv, argc, 2);
  else
    str = NULL;

  /* When community_list_set() return nevetive value, it means
     malformed community string.  */
  ret = community_list_set (bgp_clist, argv[0], str, direct, style);

  /* Free temporary community list string allocated by
     argv_concat().  */
  if (str)
    XFREE (MTYPE_TMP, str);

  if (ret < 0)
    {
      /* Display error string.  */
      community_list_perror (vty, ret);
      return CMD_WARNING;
    }

  return CMD_SUCCESS;
}