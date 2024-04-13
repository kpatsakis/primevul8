XML_GetParsingStatus(XML_Parser parser, XML_ParsingStatus *status) {
  if (parser == NULL)
    return;
  assert(status != NULL);
  *status = parser->m_parsingStatus;
}