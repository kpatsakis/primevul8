_asn1_hierarchical_name (asn1_node node, char *name, int name_size)
{
  asn1_node p;
  char tmp_name[64];

  p = node;

  name[0] = 0;

  while (p != NULL)
    {
      if (p->name[0] != 0)
	{
	  _asn1_str_cpy (tmp_name, sizeof (tmp_name), name),
	    _asn1_str_cpy (name, name_size, p->name);
	  _asn1_str_cat (name, name_size, ".");
	  _asn1_str_cat (name, name_size, tmp_name);
	}
      p = _asn1_find_up (p);
    }

  if (name[0] == 0)
    _asn1_str_cpy (name, name_size, "ROOT");
}