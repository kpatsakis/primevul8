handle_save_error (int      err_code,
		   GError **error)
{
  switch (err_code)
    {
    case errNone:
      break;
    case errOpenFile:
      g_set_error (error, POPPLER_ERROR,
		   POPPLER_ERROR_OPEN_FILE,
		   "Failed to open file for writing");
      break;
    case errEncrypted:
      g_set_error (error, POPPLER_ERROR,
		   POPPLER_ERROR_ENCRYPTED,
		   "Document is encrypted");
      break;
    default:
      g_set_error (error, POPPLER_ERROR,
		   POPPLER_ERROR_INVALID,
		   "Failed to save document");
    }

  return err_code == errNone;
}