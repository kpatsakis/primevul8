bgp_attr_dup (struct attr *new, struct attr *orig)
{
  *new = *orig;
  if (orig->extra)
    {
      new->extra = bgp_attr_extra_new();
      *new->extra = *orig->extra;
    }
}