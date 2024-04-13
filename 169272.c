bgp_attr_extra_get (struct attr *attr)
{
  if (!attr->extra)
    attr->extra = bgp_attr_extra_new();
  return attr->extra;
}