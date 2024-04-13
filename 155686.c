XML_GetInputContext(XML_Parser parser, int *offset, int *size) {
#ifdef XML_CONTEXT_BYTES
  if (parser == NULL)
    return NULL;
  if (parser->m_eventPtr && parser->m_buffer) {
    if (offset != NULL)
      *offset = (int)(parser->m_eventPtr - parser->m_buffer);
    if (size != NULL)
      *size = (int)(parser->m_bufferEnd - parser->m_buffer);
    return parser->m_buffer;
  }
#else
  (void)parser;
  (void)offset;
  (void)size;
#endif /* defined XML_CONTEXT_BYTES */
  return (const char *)0;
}