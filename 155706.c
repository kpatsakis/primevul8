XML_SetSkippedEntityHandler(XML_Parser parser,
                            XML_SkippedEntityHandler handler) {
  if (parser != NULL)
    parser->m_skippedEntityHandler = handler;
}