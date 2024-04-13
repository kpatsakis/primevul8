closezlog (struct zlog *zl)
{
  closelog();
  fclose (zl->fp);

  XFREE (MTYPE_ZLOG, zl);
}