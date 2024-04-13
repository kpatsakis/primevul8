XML_GetCurrentByteIndex(XML_Parser parser) {
  if (parser == NULL)
    return -1;
  if (parser->m_eventPtr)
    return (XML_Index)(parser->m_parseEndByteIndex
                       - (parser->m_parseEndPtr - parser->m_eventPtr));
  return -1;
}