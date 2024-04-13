attr_show_all (struct vty *vty)
{
  hash_iterate (attrhash, 
		(void (*)(struct hash_backet *, void *))
		attr_show_all_iterator,
		vty);
}