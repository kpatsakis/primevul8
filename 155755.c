XML_ParseBuffer(XML_Parser parser, int len, int isFinal) {
  const char *start;
  enum XML_Status result = XML_STATUS_OK;

  if (parser == NULL)
    return XML_STATUS_ERROR;
  switch (parser->m_parsingStatus.parsing) {
  case XML_SUSPENDED:
    parser->m_errorCode = XML_ERROR_SUSPENDED;
    return XML_STATUS_ERROR;
  case XML_FINISHED:
    parser->m_errorCode = XML_ERROR_FINISHED;
    return XML_STATUS_ERROR;
  case XML_INITIALIZED:
    /* Has someone called XML_GetBuffer successfully before? */
    if (! parser->m_bufferPtr) {
      parser->m_errorCode = XML_ERROR_NO_BUFFER;
      return XML_STATUS_ERROR;
    }

    if (parser->m_parentParser == NULL && ! startParsing(parser)) {
      parser->m_errorCode = XML_ERROR_NO_MEMORY;
      return XML_STATUS_ERROR;
    }
    /* fall through */
  default:
    parser->m_parsingStatus.parsing = XML_PARSING;
  }

  start = parser->m_bufferPtr;
  parser->m_positionPtr = start;
  parser->m_bufferEnd += len;
  parser->m_parseEndPtr = parser->m_bufferEnd;
  parser->m_parseEndByteIndex += len;
  parser->m_parsingStatus.finalBuffer = (XML_Bool)isFinal;

  parser->m_errorCode = parser->m_processor(
      parser, start, parser->m_parseEndPtr, &parser->m_bufferPtr);

  if (parser->m_errorCode != XML_ERROR_NONE) {
    parser->m_eventEndPtr = parser->m_eventPtr;
    parser->m_processor = errorProcessor;
    return XML_STATUS_ERROR;
  } else {
    switch (parser->m_parsingStatus.parsing) {
    case XML_SUSPENDED:
      result = XML_STATUS_SUSPENDED;
      break;
    case XML_INITIALIZED:
    case XML_PARSING:
      if (isFinal) {
        parser->m_parsingStatus.parsing = XML_FINISHED;
        return result;
      }
    default:; /* should not happen */
    }
  }

  XmlUpdatePosition(parser->m_encoding, parser->m_positionPtr,
                    parser->m_bufferPtr, &parser->m_position);
  parser->m_positionPtr = parser->m_bufferPtr;
  return result;
}