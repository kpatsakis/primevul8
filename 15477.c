community_show_all_iterator (struct hash_backet *backet, struct vty *vty)
{
  struct community *com;

  com = (struct community *) backet->data;
  vty_out (vty, "[%p] (%ld) %s%s", backet, com->refcnt,
	   community_str (com), VTY_NEWLINE);
}