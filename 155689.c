XML_SetUnknownEncodingHandler(XML_Parser parser,
                              XML_UnknownEncodingHandler handler, void *data) {
  if (parser == NULL)
    return;
  parser->m_unknownEncodingHandler = handler;
  parser->m_unknownEncodingHandlerData = data;
}