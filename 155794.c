entityValueProcessor(XML_Parser parser, const char *s, const char *end,
                     const char **nextPtr) {
  const char *start = s;
  const char *next = s;
  const ENCODING *enc = parser->m_encoding;
  int tok;

  for (;;) {
    tok = XmlPrologTok(enc, start, end, &next);
    /* Note: These bytes are accounted later in:
             - storeEntityValue
    */
    if (tok <= 0) {
      if (! parser->m_parsingStatus.finalBuffer && tok != XML_TOK_INVALID) {
        *nextPtr = s;
        return XML_ERROR_NONE;
      }
      switch (tok) {
      case XML_TOK_INVALID:
        return XML_ERROR_INVALID_TOKEN;
      case XML_TOK_PARTIAL:
        return XML_ERROR_UNCLOSED_TOKEN;
      case XML_TOK_PARTIAL_CHAR:
        return XML_ERROR_PARTIAL_CHAR;
      case XML_TOK_NONE: /* start == end */
      default:
        break;
      }
      /* found end of entity value - can store it now */
      return storeEntityValue(parser, enc, s, end, XML_ACCOUNT_DIRECT);
    }
    start = next;
  }
}