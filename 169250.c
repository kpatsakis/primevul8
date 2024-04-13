bgp_attr_default_intern (u_char origin)
{
  struct attr attr;
  struct attr *new;
  
  memset (&attr, 0, sizeof (struct attr));
  bgp_attr_extra_get (&attr);
  
  bgp_attr_default_set(&attr, origin);

  new = bgp_attr_intern (&attr);
  bgp_attr_extra_free (&attr);
  
  aspath_unintern (&new->aspath);
  return new;
}