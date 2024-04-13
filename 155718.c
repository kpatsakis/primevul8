handleUnknownEncoding(XML_Parser parser, const XML_Char *encodingName) {
  if (parser->m_unknownEncodingHandler) {
    XML_Encoding info;
    int i;
    for (i = 0; i < 256; i++)
      info.map[i] = -1;
    info.convert = NULL;
    info.data = NULL;
    info.release = NULL;
    if (parser->m_unknownEncodingHandler(parser->m_unknownEncodingHandlerData,
                                         encodingName, &info)) {
      ENCODING *enc;
      parser->m_unknownEncodingMem = MALLOC(parser, XmlSizeOfUnknownEncoding());
      if (! parser->m_unknownEncodingMem) {
        if (info.release)
          info.release(info.data);
        return XML_ERROR_NO_MEMORY;
      }
      enc = (parser->m_ns ? XmlInitUnknownEncodingNS : XmlInitUnknownEncoding)(
          parser->m_unknownEncodingMem, info.map, info.convert, info.data);
      if (enc) {
        parser->m_unknownEncodingData = info.data;
        parser->m_unknownEncodingRelease = info.release;
        parser->m_encoding = enc;
        return XML_ERROR_NONE;
      }
    }
    if (info.release != NULL)
      info.release(info.data);
  }
  return XML_ERROR_UNKNOWN_ENCODING;
}