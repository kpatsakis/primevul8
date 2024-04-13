save_helper (const gchar  *buf,
	     gsize         count,
	     gpointer      data,
	     GError      **error)
{
  FILE *f = (FILE *) data;
  gsize n;

  n = fwrite (buf, 1, count, f);
  if (n != count)
    {
      g_set_error (error,
		   G_FILE_ERROR,
		   g_file_error_from_errno (errno),
		   _("Error writing to image file: %s"),
		   g_strerror (errno));
      return FALSE;
    }

  return TRUE;
}