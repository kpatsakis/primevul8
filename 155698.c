XML_GetCurrentColumnNumber(XML_Parser parser) {
  if (parser == NULL)
    return 0;
  if (parser->m_eventPtr && parser->m_eventPtr >= parser->m_positionPtr) {
    XmlUpdatePosition(parser->m_encoding, parser->m_positionPtr,
                      parser->m_eventPtr, &parser->m_position);
    parser->m_positionPtr = parser->m_eventPtr;
  }
  return parser->m_position.columnNumber;
}