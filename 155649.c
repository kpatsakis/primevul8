XML_SetEncoding(XML_Parser parser, const XML_Char *encodingName) {
  if (parser == NULL)
    return XML_STATUS_ERROR;
  /* Block after XML_Parse()/XML_ParseBuffer() has been called.
     XXX There's no way for the caller to determine which of the
     XXX possible error cases caused the XML_STATUS_ERROR return.
  */
  if (parser->m_parsingStatus.parsing == XML_PARSING
      || parser->m_parsingStatus.parsing == XML_SUSPENDED)
    return XML_STATUS_ERROR;

  /* Get rid of any previous encoding name */
  FREE(parser, (void *)parser->m_protocolEncodingName);

  if (encodingName == NULL)
    /* No new encoding name */
    parser->m_protocolEncodingName = NULL;
  else {
    /* Copy the new encoding name into allocated memory */
    parser->m_protocolEncodingName = copyString(encodingName, &(parser->m_mem));
    if (! parser->m_protocolEncodingName)
      return XML_STATUS_ERROR;
  }
  return XML_STATUS_OK;
}