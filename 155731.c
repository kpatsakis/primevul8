doIgnoreSection(XML_Parser parser, const ENCODING *enc, const char **startPtr,
                const char *end, const char **nextPtr, XML_Bool haveMore) {
  const char *next = *startPtr; /* in case of XML_TOK_NONE or XML_TOK_PARTIAL */
  int tok;
  const char *s = *startPtr;
  const char **eventPP;
  const char **eventEndPP;
  if (enc == parser->m_encoding) {
    eventPP = &parser->m_eventPtr;
    *eventPP = s;
    eventEndPP = &parser->m_eventEndPtr;
  } else {
    /* It's not entirely clear, but it seems the following two lines
     * of code cannot be executed.  The only occasions on which 'enc'
     * is not 'encoding' are when this function is called
     * from the internal entity processing, and IGNORE sections are an
     * error in internal entities.
     *
     * Since it really isn't clear that this is true, we keep the code
     * and just remove it from our coverage tests.
     *
     * LCOV_EXCL_START
     */
    eventPP = &(parser->m_openInternalEntities->internalEventPtr);
    eventEndPP = &(parser->m_openInternalEntities->internalEventEndPtr);
    /* LCOV_EXCL_STOP */
  }
  *eventPP = s;
  *startPtr = NULL;
  tok = XmlIgnoreSectionTok(enc, s, end, &next);
#  ifdef XML_DTD
  if (! accountingDiffTolerated(parser, tok, s, next, __LINE__,
                                XML_ACCOUNT_DIRECT)) {
    accountingOnAbort(parser);
    return XML_ERROR_AMPLIFICATION_LIMIT_BREACH;
  }
#  endif
  *eventEndPP = next;
  switch (tok) {
  case XML_TOK_IGNORE_SECT:
    if (parser->m_defaultHandler)
      reportDefault(parser, enc, s, next);
    *startPtr = next;
    *nextPtr = next;
    if (parser->m_parsingStatus.parsing == XML_FINISHED)
      return XML_ERROR_ABORTED;
    else
      return XML_ERROR_NONE;
  case XML_TOK_INVALID:
    *eventPP = next;
    return XML_ERROR_INVALID_TOKEN;
  case XML_TOK_PARTIAL_CHAR:
    if (haveMore) {
      *nextPtr = s;
      return XML_ERROR_NONE;
    }
    return XML_ERROR_PARTIAL_CHAR;
  case XML_TOK_PARTIAL:
  case XML_TOK_NONE:
    if (haveMore) {
      *nextPtr = s;
      return XML_ERROR_NONE;
    }
    return XML_ERROR_SYNTAX; /* XML_ERROR_UNCLOSED_IGNORE_SECTION */
  default:
    /* All of the tokens that XmlIgnoreSectionTok() returns have
     * explicit cases to handle them, so this default case is never
     * executed.  We keep it as a safety net anyway, and remove it
     * from our test coverage statistics.
     *
     * LCOV_EXCL_START
     */
    *eventPP = next;
    return XML_ERROR_UNEXPECTED_STATE;
    /* LCOV_EXCL_STOP */
  }
  /* not reached */
}