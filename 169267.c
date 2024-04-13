bgp_attr_extra_new (void)
{
  return XCALLOC (MTYPE_ATTR_EXTRA, sizeof (struct attr_extra));
}