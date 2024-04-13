XML_UseParserAsHandlerArg(XML_Parser parser) {
  if (parser != NULL)
    parser->m_handlerArg = parser;
}