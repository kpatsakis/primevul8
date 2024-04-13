XML_UseForeignDTD(XML_Parser parser, XML_Bool useDTD) {
  if (parser == NULL)
    return XML_ERROR_INVALID_ARGUMENT;
#ifdef XML_DTD
  /* block after XML_Parse()/XML_ParseBuffer() has been called */
  if (parser->m_parsingStatus.parsing == XML_PARSING
      || parser->m_parsingStatus.parsing == XML_SUSPENDED)
    return XML_ERROR_CANT_CHANGE_FEATURE_ONCE_PARSING;
  parser->m_useForeignDTD = useDTD;
  return XML_ERROR_NONE;
#else
  UNUSED_P(useDTD);
  return XML_ERROR_FEATURE_REQUIRES_XML_DTD;
#endif
}