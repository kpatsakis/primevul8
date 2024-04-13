XML_DefaultCurrent(XML_Parser parser) {
  if (parser == NULL)
    return;
  if (parser->m_defaultHandler) {
    if (parser->m_openInternalEntities)
      reportDefault(parser, parser->m_internalEncoding,
                    parser->m_openInternalEntities->internalEventPtr,
                    parser->m_openInternalEntities->internalEventEndPtr);
    else
      reportDefault(parser, parser->m_encoding, parser->m_eventPtr,
                    parser->m_eventEndPtr);
  }
}