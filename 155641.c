ignoreSectionProcessor(XML_Parser parser, const char *start, const char *end,
                       const char **endPtr) {
  enum XML_Error result
      = doIgnoreSection(parser, parser->m_encoding, &start, end, endPtr,
                        (XML_Bool)! parser->m_parsingStatus.finalBuffer);
  if (result != XML_ERROR_NONE)
    return result;
  if (start) {
    parser->m_processor = prologProcessor;
    return prologProcessor(parser, start, end, endPtr);
  }
  return result;
}