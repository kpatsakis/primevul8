static void free_fbuffer(imp_sth_fbh_t *fbh)
{
  if (fbh)
    Safefree(fbh);
}