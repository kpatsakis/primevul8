externalEntityInitProcessor3(XML_Parser parser, const char *start,
                             const char *end, const char **endPtr) {
  int tok;
  const char *next = start; /* XmlContentTok doesn't always set the last arg */
  parser->m_eventPtr = start;
  tok = XmlContentTok(parser->m_encoding, start, end, &next);
  /* Note: These bytes are accounted later in:
           - processXmlDecl
           - externalEntityContentProcessor
  */
  parser->m_eventEndPtr = next;

  switch (tok) {
  case XML_TOK_XML_DECL: {
    enum XML_Error result;
    result = processXmlDecl(parser, 1, start, next);
    if (result != XML_ERROR_NONE)
      return result;
    switch (parser->m_parsingStatus.parsing) {
    case XML_SUSPENDED:
      *endPtr = next;
      return XML_ERROR_NONE;
    case XML_FINISHED:
      return XML_ERROR_ABORTED;
    default:
      start = next;
    }
  } break;
  case XML_TOK_PARTIAL:
    if (! parser->m_parsingStatus.finalBuffer) {
      *endPtr = start;
      return XML_ERROR_NONE;
    }
    return XML_ERROR_UNCLOSED_TOKEN;
  case XML_TOK_PARTIAL_CHAR:
    if (! parser->m_parsingStatus.finalBuffer) {
      *endPtr = start;
      return XML_ERROR_NONE;
    }
    return XML_ERROR_PARTIAL_CHAR;
  }
  parser->m_processor = externalEntityContentProcessor;
  parser->m_tagLevel = 1;
  return externalEntityContentProcessor(parser, start, end, endPtr);
}