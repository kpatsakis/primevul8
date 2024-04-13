static CURLcode file_setup_connection(struct connectdata *conn)
{
  /* allocate the FILE specific struct */
  conn->data->req.protop = calloc(1, sizeof(struct FILEPROTO));
  if(!conn->data->req.protop)
    return CURLE_OUT_OF_MEMORY;

  return CURLE_OK;
}