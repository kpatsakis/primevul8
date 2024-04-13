try_fseek (FILE  *f,
           glong  pos,
           gint   whence)
{
  if (fseek (f, pos, whence) < 0)
    {
      g_message ("Seek error: %s", g_strerror (errno));
      fclose (f);
      return -1;
    }
  return 0;
}