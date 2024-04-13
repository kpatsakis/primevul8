XML_StopParser(XML_Parser parser, XML_Bool resumable) {
  if (parser == NULL)
    return XML_STATUS_ERROR;
  switch (parser->m_parsingStatus.parsing) {
  case XML_SUSPENDED:
    if (resumable) {
      parser->m_errorCode = XML_ERROR_SUSPENDED;
      return XML_STATUS_ERROR;
    }
    parser->m_parsingStatus.parsing = XML_FINISHED;
    break;
  case XML_FINISHED:
    parser->m_errorCode = XML_ERROR_FINISHED;
    return XML_STATUS_ERROR;
  default:
    if (resumable) {
#ifdef XML_DTD
      if (parser->m_isParamEntity) {
        parser->m_errorCode = XML_ERROR_SUSPEND_PE;
        return XML_STATUS_ERROR;
      }
#endif
      parser->m_parsingStatus.parsing = XML_SUSPENDED;
    } else
      parser->m_parsingStatus.parsing = XML_FINISHED;
  }
  return XML_STATUS_OK;
}