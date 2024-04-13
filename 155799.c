externalEntityContentProcessor(XML_Parser parser, const char *start,
                               const char *end, const char **endPtr) {
  enum XML_Error result
      = doContent(parser, 1, parser->m_encoding, start, end, endPtr,
                  (XML_Bool)! parser->m_parsingStatus.finalBuffer,
                  XML_ACCOUNT_ENTITY_EXPANSION);
  if (result == XML_ERROR_NONE) {
    if (! storeRawNames(parser))
      return XML_ERROR_NO_MEMORY;
  }
  return result;
}