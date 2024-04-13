XML_SetProcessingInstructionHandler(XML_Parser parser,
                                    XML_ProcessingInstructionHandler handler) {
  if (parser != NULL)
    parser->m_processingInstructionHandler = handler;
}