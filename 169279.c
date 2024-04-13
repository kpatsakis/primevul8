bgp_attr_unintern (struct attr **attr)
{
  struct attr *ret;
  struct attr tmp;
  
  /* Decrement attribute reference. */
  (*attr)->refcnt--;
  
  tmp = *(*attr);
  
  if ((*attr)->extra)
    {
      tmp.extra = bgp_attr_extra_new ();
      memcpy (tmp.extra, (*attr)->extra, sizeof (struct attr_extra));
    }
  
  /* If reference becomes zero then free attribute object. */
  if ((*attr)->refcnt == 0)
    {    
      ret = hash_release (attrhash, *attr);
      assert (ret != NULL);
      bgp_attr_extra_free (*attr);
      XFREE (MTYPE_ATTR, *attr);
      *attr = NULL;
    }

  bgp_attr_unintern_sub (&tmp);
  bgp_attr_extra_free (&tmp);
}