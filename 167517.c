poppler_attachment_save (PopplerAttachment  *attachment,
			 const char         *filename,
			 GError            **error)
{
  gboolean result;
  FILE *f;
  
  g_return_val_if_fail (POPPLER_IS_ATTACHMENT (attachment), FALSE);

  f = g_fopen (filename, "wb");

  if (f == nullptr)
    {
      gchar *display_name = g_filename_display_name (filename);
      g_set_error (error,
		   G_FILE_ERROR,
		   g_file_error_from_errno (errno),
		   _("Failed to open '%s' for writing: %s"),
		   display_name,
		   g_strerror (errno));
      g_free (display_name);
      return FALSE;
    }

  result = poppler_attachment_save_to_callback (attachment, save_helper, f, error);

  if (fclose (f) < 0)
    {
      gchar *display_name = g_filename_display_name (filename);
      g_set_error (error,
		   G_FILE_ERROR,
		   g_file_error_from_errno (errno),
		   _("Failed to close '%s', all data may not have been saved: %s"),
		   display_name,
		   g_strerror (errno));
      g_free (display_name);
      return FALSE;
    }

  return result;
}