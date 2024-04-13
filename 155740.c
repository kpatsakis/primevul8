doCdataSection(XML_Parser parser, const ENCODING *enc, const char **startPtr,
               const char *end, const char **nextPtr, XML_Bool haveMore,
               enum XML_Account account) {
  const char *s = *startPtr;
  const char **eventPP;
  const char **eventEndPP;
  if (enc == parser->m_encoding) {
    eventPP = &parser->m_eventPtr;
    *eventPP = s;
    eventEndPP = &parser->m_eventEndPtr;
  } else {
    eventPP = &(parser->m_openInternalEntities->internalEventPtr);
    eventEndPP = &(parser->m_openInternalEntities->internalEventEndPtr);
  }
  *eventPP = s;
  *startPtr = NULL;

  for (;;) {
    const char *next = s; /* in case of XML_TOK_NONE or XML_TOK_PARTIAL */
    int tok = XmlCdataSectionTok(enc, s, end, &next);
#ifdef XML_DTD
    if (! accountingDiffTolerated(parser, tok, s, next, __LINE__, account)) {
      accountingOnAbort(parser);
      return XML_ERROR_AMPLIFICATION_LIMIT_BREACH;
    }
#else
    UNUSED_P(account);
#endif
    *eventEndPP = next;
    switch (tok) {
    case XML_TOK_CDATA_SECT_CLOSE:
      if (parser->m_endCdataSectionHandler)
        parser->m_endCdataSectionHandler(parser->m_handlerArg);
      /* BEGIN disabled code */
      /* see comment under XML_TOK_CDATA_SECT_OPEN */
      else if (0 && parser->m_characterDataHandler)
        parser->m_characterDataHandler(parser->m_handlerArg, parser->m_dataBuf,
                                       0);
      /* END disabled code */
      else if (parser->m_defaultHandler)
        reportDefault(parser, enc, s, next);
      *startPtr = next;
      *nextPtr = next;
      if (parser->m_parsingStatus.parsing == XML_FINISHED)
        return XML_ERROR_ABORTED;
      else
        return XML_ERROR_NONE;
    case XML_TOK_DATA_NEWLINE:
      if (parser->m_characterDataHandler) {
        XML_Char c = 0xA;
        parser->m_characterDataHandler(parser->m_handlerArg, &c, 1);
      } else if (parser->m_defaultHandler)
        reportDefault(parser, enc, s, next);
      break;
    case XML_TOK_DATA_CHARS: {
      XML_CharacterDataHandler charDataHandler = parser->m_characterDataHandler;
      if (charDataHandler) {
        if (MUST_CONVERT(enc, s)) {
          for (;;) {
            ICHAR *dataPtr = (ICHAR *)parser->m_dataBuf;
            const enum XML_Convert_Result convert_res = XmlConvert(
                enc, &s, next, &dataPtr, (ICHAR *)parser->m_dataBufEnd);
            *eventEndPP = next;
            charDataHandler(parser->m_handlerArg, parser->m_dataBuf,
                            (int)(dataPtr - (ICHAR *)parser->m_dataBuf));
            if ((convert_res == XML_CONVERT_COMPLETED)
                || (convert_res == XML_CONVERT_INPUT_INCOMPLETE))
              break;
            *eventPP = s;
          }
        } else
          charDataHandler(parser->m_handlerArg, (XML_Char *)s,
                          (int)((XML_Char *)next - (XML_Char *)s));
      } else if (parser->m_defaultHandler)
        reportDefault(parser, enc, s, next);
    } break;
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
      return XML_ERROR_UNCLOSED_CDATA_SECTION;
    default:
      /* Every token returned by XmlCdataSectionTok() has its own
       * explicit case, so this default case will never be executed.
       * We retain it as a safety net and exclude it from the coverage
       * statistics.
       *
       * LCOV_EXCL_START
       */
      *eventPP = next;
      return XML_ERROR_UNEXPECTED_STATE;
      /* LCOV_EXCL_STOP */
    }

    *eventPP = s = next;
    switch (parser->m_parsingStatus.parsing) {
    case XML_SUSPENDED:
      *nextPtr = next;
      return XML_ERROR_NONE;
    case XML_FINISHED:
      return XML_ERROR_ABORTED;
    default:;
    }
  }
  /* not reached */
}