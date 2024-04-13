poppler_attachment_save_to_callback (PopplerAttachment          *attachment,
				     PopplerAttachmentSaveFunc   save_func,
				     gpointer                    user_data,
				     GError                    **error)
{
  Stream *stream;
  gchar buf[BUF_SIZE]; 
  int i;
  gboolean eof_reached = FALSE;

  g_return_val_if_fail (POPPLER_IS_ATTACHMENT (attachment), FALSE);

  stream = POPPLER_ATTACHMENT_GET_PRIVATE (attachment)->obj_stream.getStream();
  stream->reset();

  do
    {
      int data;

      for (i = 0; i < BUF_SIZE; i++)
	{
	  data = stream->getChar ();
	  if (data == EOF)
	    {
	      eof_reached = TRUE;
	      break;
	    }
	  buf[i] = data;
	}

      if (i > 0)
	{
	  if (! (save_func) (buf, i, user_data, error))
	    return FALSE;
	}
    }
  while (! eof_reached);


  return TRUE;
}