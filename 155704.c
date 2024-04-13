entityValueInitProcessor(XML_Parser parser, const char *s, const char *end,
                         const char **nextPtr) {
  int tok;
  const char *start = s;
  const char *next = start;
  parser->m_eventPtr = start;

  for (;;) {
    tok = XmlPrologTok(parser->m_encoding, start, end, &next);
    /* Note: Except for XML_TOK_BOM below, these bytes are accounted later in:
             - storeEntityValue
             - processXmlDecl
    */
    parser->m_eventEndPtr = next;
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
      return storeEntityValue(parser, parser->m_encoding, s, end,
                              XML_ACCOUNT_DIRECT);
    } else if (tok == XML_TOK_XML_DECL) {
      enum XML_Error result;
      result = processXmlDecl(parser, 0, start, next);
      if (result != XML_ERROR_NONE)
        return result;
      /* At this point, m_parsingStatus.parsing cannot be XML_SUSPENDED.  For
       * that to happen, a parameter entity parsing handler must have attempted
       * to suspend the parser, which fails and raises an error.  The parser can
       * be aborted, but can't be suspended.
       */
      if (parser->m_parsingStatus.parsing == XML_FINISHED)
        return XML_ERROR_ABORTED;
      *nextPtr = next;
      /* stop scanning for text declaration - we found one */
      parser->m_processor = entityValueProcessor;
      return entityValueProcessor(parser, next, end, nextPtr);
    }
    /* If we are at the end of the buffer, this would cause XmlPrologTok to
       return XML_TOK_NONE on the next call, which would then cause the
       function to exit with *nextPtr set to s - that is what we want for other
       tokens, but not for the BOM - we would rather like to skip it;
       then, when this routine is entered the next time, XmlPrologTok will
       return XML_TOK_INVALID, since the BOM is still in the buffer
    */
    else if (tok == XML_TOK_BOM && next == end
             && ! parser->m_parsingStatus.finalBuffer) {
#  ifdef XML_DTD
      if (! accountingDiffTolerated(parser, tok, s, next, __LINE__,
                                    XML_ACCOUNT_DIRECT)) {
        accountingOnAbort(parser);
        return XML_ERROR_AMPLIFICATION_LIMIT_BREACH;
      }
#  endif

      *nextPtr = next;
      return XML_ERROR_NONE;
    }
    /* If we get this token, we have the start of what might be a
       normal tag, but not a declaration (i.e. it doesn't begin with
       "<!").  In a DTD context, that isn't legal.
    */
    else if (tok == XML_TOK_INSTANCE_START) {
      *nextPtr = next;
      return XML_ERROR_SYNTAX;
    }
    start = next;
    parser->m_eventPtr = start;
  }
}