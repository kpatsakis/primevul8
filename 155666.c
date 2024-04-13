epilogProcessor(XML_Parser parser, const char *s, const char *end,
                const char **nextPtr) {
  parser->m_processor = epilogProcessor;
  parser->m_eventPtr = s;
  for (;;) {
    const char *next = NULL;
    int tok = XmlPrologTok(parser->m_encoding, s, end, &next);
#ifdef XML_DTD
    if (! accountingDiffTolerated(parser, tok, s, next, __LINE__,
                                  XML_ACCOUNT_DIRECT)) {
      accountingOnAbort(parser);
      return XML_ERROR_AMPLIFICATION_LIMIT_BREACH;
    }
#endif
    parser->m_eventEndPtr = next;
    switch (tok) {
    /* report partial linebreak - it might be the last token */
    case -XML_TOK_PROLOG_S:
      if (parser->m_defaultHandler) {
        reportDefault(parser, parser->m_encoding, s, next);
        if (parser->m_parsingStatus.parsing == XML_FINISHED)
          return XML_ERROR_ABORTED;
      }
      *nextPtr = next;
      return XML_ERROR_NONE;
    case XML_TOK_NONE:
      *nextPtr = s;
      return XML_ERROR_NONE;
    case XML_TOK_PROLOG_S:
      if (parser->m_defaultHandler)
        reportDefault(parser, parser->m_encoding, s, next);
      break;
    case XML_TOK_PI:
      if (! reportProcessingInstruction(parser, parser->m_encoding, s, next))
        return XML_ERROR_NO_MEMORY;
      break;
    case XML_TOK_COMMENT:
      if (! reportComment(parser, parser->m_encoding, s, next))
        return XML_ERROR_NO_MEMORY;
      break;
    case XML_TOK_INVALID:
      parser->m_eventPtr = next;
      return XML_ERROR_INVALID_TOKEN;
    case XML_TOK_PARTIAL:
      if (! parser->m_parsingStatus.finalBuffer) {
        *nextPtr = s;
        return XML_ERROR_NONE;
      }
      return XML_ERROR_UNCLOSED_TOKEN;
    case XML_TOK_PARTIAL_CHAR:
      if (! parser->m_parsingStatus.finalBuffer) {
        *nextPtr = s;
        return XML_ERROR_NONE;
      }
      return XML_ERROR_PARTIAL_CHAR;
    default:
      return XML_ERROR_JUNK_AFTER_DOC_ELEMENT;
    }
    parser->m_eventPtr = s = next;
    switch (parser->m_parsingStatus.parsing) {
    case XML_SUSPENDED:
      *nextPtr = next;
      return XML_ERROR_NONE;
    case XML_FINISHED:
      return XML_ERROR_ABORTED;
    default:;
    }
  }
}