XML_SetBase(XML_Parser parser, const XML_Char *p) {
  if (parser == NULL)
    return XML_STATUS_ERROR;
  if (p) {
    p = poolCopyString(&parser->m_dtd->pool, p);
    if (! p)
      return XML_STATUS_ERROR;
    parser->m_curBase = p;
  } else
    parser->m_curBase = NULL;
  return XML_STATUS_OK;
}