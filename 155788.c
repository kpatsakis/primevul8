cdataSectionProcessor(XML_Parser parser, const char *start, const char *end,
                      const char **endPtr) {
  enum XML_Error result = doCdataSection(
      parser, parser->m_encoding, &start, end, endPtr,
      (XML_Bool)! parser->m_parsingStatus.finalBuffer, XML_ACCOUNT_DIRECT);
  if (result != XML_ERROR_NONE)
    return result;
  if (start) {
    if (parser->m_parentParser) { /* we are parsing an external entity */
      parser->m_processor = externalEntityContentProcessor;
      return externalEntityContentProcessor(parser, start, end, endPtr);
    } else {
      parser->m_processor = contentProcessor;
      return contentProcessor(parser, start, end, endPtr);
    }
  }
  return result;
}