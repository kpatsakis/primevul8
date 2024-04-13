XML_SetParamEntityParsing(XML_Parser parser,
                          enum XML_ParamEntityParsing peParsing) {
  if (parser == NULL)
    return 0;
  /* block after XML_Parse()/XML_ParseBuffer() has been called */
  if (parser->m_parsingStatus.parsing == XML_PARSING
      || parser->m_parsingStatus.parsing == XML_SUSPENDED)
    return 0;
#ifdef XML_DTD
  parser->m_paramEntityParsing = peParsing;
  return 1;
#else
  return peParsing == XML_PARAM_ENTITY_PARSING_NEVER;
#endif
}