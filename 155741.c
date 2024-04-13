XML_SetExternalEntityRefHandlerArg(XML_Parser parser, void *arg) {
  if (parser == NULL)
    return;
  if (arg)
    parser->m_externalEntityRefHandlerArg = (XML_Parser)arg;
  else
    parser->m_externalEntityRefHandlerArg = parser;
}