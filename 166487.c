static CURLcode file_done(struct connectdata *conn,
                               CURLcode status, bool premature)
{
  struct FILEPROTO *file = conn->data->req.protop;
  (void)status; /* not used */
  (void)premature; /* not used */

  if(file) {
    Curl_safefree(file->freepath);
    file->path = NULL;
    if(file->fd != -1)
      close(file->fd);
    file->fd = -1;
  }

  return CURLE_OK;
}