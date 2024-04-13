int dbd_st_blob_read (
  SV *sth,
  imp_sth_t *imp_sth,
  int field,
  long offset,
  long len,
  SV *destrv,
  long destoffset)
{
    /* quell warnings */
    sth= sth;
    imp_sth=imp_sth;
    field= field;
    offset= offset;
    len= len;
    destrv= destrv;
    destoffset= destoffset;
    return FALSE;
}