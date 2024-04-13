onig_region_new(void)
{
  OnigRegion* r;

  r = (OnigRegion* )xmalloc(sizeof(OnigRegion));
  CHECK_NULL_RETURN(r);
  onig_region_init(r);
  return r;
}