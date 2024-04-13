community_list_perror (struct vty *vty, int ret)
{
  switch (ret)
    {
    case COMMUNITY_LIST_ERR_CANT_FIND_LIST:
      vty_out (vty, "%% Can't find communit-list%s", VTY_NEWLINE);
      break;
    case COMMUNITY_LIST_ERR_MALFORMED_VAL:
      vty_out (vty, "%% Malformed community-list value%s", VTY_NEWLINE);
      break;
    case COMMUNITY_LIST_ERR_STANDARD_CONFLICT:
      vty_out (vty, "%% Community name conflict, previously defined as standard community%s", VTY_NEWLINE);
      break;
    case COMMUNITY_LIST_ERR_EXPANDED_CONFLICT:
      vty_out (vty, "%% Community name conflict, previously defined as expanded community%s", VTY_NEWLINE);
      break;
    }
}