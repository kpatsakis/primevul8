reportComment(XML_Parser parser, const ENCODING *enc, const char *start,
              const char *end) {
  XML_Char *data;
  if (! parser->m_commentHandler) {
    if (parser->m_defaultHandler)
      reportDefault(parser, enc, start, end);
    return 1;
  }
  data = poolStoreString(&parser->m_tempPool, enc,
                         start + enc->minBytesPerChar * 4,
                         end - enc->minBytesPerChar * 3);
  if (! data)
    return 0;
  normalizeLines(data);
  parser->m_commentHandler(parser->m_handlerArg, data);
  poolClear(&parser->m_tempPool);
  return 1;
}