static void free_fbind(imp_sth_phb_t *fbind)
{
  if (fbind)
    Safefree(fbind);
}