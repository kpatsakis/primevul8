reportProcessingInstruction(XML_Parser parser, const ENCODING *enc,
                            const char *start, const char *end) {
  const XML_Char *target;
  XML_Char *data;
  const char *tem;
  if (! parser->m_processingInstructionHandler) {
    if (parser->m_defaultHandler)
      reportDefault(parser, enc, start, end);
    return 1;
  }
  start += enc->minBytesPerChar * 2;
  tem = start + XmlNameLength(enc, start);
  target = poolStoreString(&parser->m_tempPool, enc, start, tem);
  if (! target)
    return 0;
  poolFinish(&parser->m_tempPool);
  data = poolStoreString(&parser->m_tempPool, enc, XmlSkipS(enc, tem),
                         end - enc->minBytesPerChar * 2);
  if (! data)
    return 0;
  normalizeLines(data);
  parser->m_processingInstructionHandler(parser->m_handlerArg, target, data);
  poolClear(&parser->m_tempPool);
  return 1;
}