externalEntityInitProcessor2(XML_Parser parser, const char *start,
                             const char *end, const char **endPtr) {
  const char *next = start; /* XmlContentTok doesn't always set the last arg */
  int tok = XmlContentTok(parser->m_encoding, start, end, &next);
  switch (tok) {
  case XML_TOK_BOM:
#ifdef XML_DTD
    if (! accountingDiffTolerated(parser, tok, start, next, __LINE__,
                                  XML_ACCOUNT_DIRECT)) {
      accountingOnAbort(parser);
      return XML_ERROR_AMPLIFICATION_LIMIT_BREACH;
    }
#endif /* XML_DTD */

    /* If we are at the end of the buffer, this would cause the next stage,
       i.e. externalEntityInitProcessor3, to pass control directly to
       doContent (by detecting XML_TOK_NONE) without processing any xml text
       declaration - causing the error XML_ERROR_MISPLACED_XML_PI in doContent.
    */
    if (next == end && ! parser->m_parsingStatus.finalBuffer) {
      *endPtr = next;
      return XML_ERROR_NONE;
    }
    start = next;
    break;
  case XML_TOK_PARTIAL:
    if (! parser->m_parsingStatus.finalBuffer) {
      *endPtr = start;
      return XML_ERROR_NONE;
    }
    parser->m_eventPtr = start;
    return XML_ERROR_UNCLOSED_TOKEN;
  case XML_TOK_PARTIAL_CHAR:
    if (! parser->m_parsingStatus.finalBuffer) {
      *endPtr = start;
      return XML_ERROR_NONE;
    }
    parser->m_eventPtr = start;
    return XML_ERROR_PARTIAL_CHAR;
  }
  parser->m_processor = externalEntityInitProcessor3;
  return externalEntityInitProcessor3(parser, start, end, endPtr);
}