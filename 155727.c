XML_ResumeParser(XML_Parser parser) {
  enum XML_Status result = XML_STATUS_OK;

  if (parser == NULL)
    return XML_STATUS_ERROR;
  if (parser->m_parsingStatus.parsing != XML_SUSPENDED) {
    parser->m_errorCode = XML_ERROR_NOT_SUSPENDED;
    return XML_STATUS_ERROR;
  }
  parser->m_parsingStatus.parsing = XML_PARSING;

  parser->m_errorCode = parser->m_processor(
      parser, parser->m_bufferPtr, parser->m_parseEndPtr, &parser->m_bufferPtr);

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
      if (parser->m_parsingStatus.finalBuffer) {
        parser->m_parsingStatus.parsing = XML_FINISHED;
        return result;
      }
    default:;
    }
  }

  XmlUpdatePosition(parser->m_encoding, parser->m_positionPtr,
                    parser->m_bufferPtr, &parser->m_position);
  parser->m_positionPtr = parser->m_bufferPtr;
  return result;
}