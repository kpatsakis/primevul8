externalParEntInitProcessor(XML_Parser parser, const char *s, const char *end,
                            const char **nextPtr) {
  enum XML_Error result = initializeEncoding(parser);
  if (result != XML_ERROR_NONE)
    return result;

  /* we know now that XML_Parse(Buffer) has been called,
     so we consider the external parameter entity read */
  parser->m_dtd->paramEntityRead = XML_TRUE;

  if (parser->m_prologState.inEntityValue) {
    parser->m_processor = entityValueInitProcessor;
    return entityValueInitProcessor(parser, s, end, nextPtr);
  } else {
    parser->m_processor = externalParEntProcessor;
    return externalParEntProcessor(parser, s, end, nextPtr);
  }
}