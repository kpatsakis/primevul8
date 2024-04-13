initializeEncoding(XML_Parser parser) {
  const char *s;
#ifdef XML_UNICODE
  char encodingBuf[128];
  /* See comments about `protocolEncodingName` in parserInit() */
  if (! parser->m_protocolEncodingName)
    s = NULL;
  else {
    int i;
    for (i = 0; parser->m_protocolEncodingName[i]; i++) {
      if (i == sizeof(encodingBuf) - 1
          || (parser->m_protocolEncodingName[i] & ~0x7f) != 0) {
        encodingBuf[0] = '\0';
        break;
      }
      encodingBuf[i] = (char)parser->m_protocolEncodingName[i];
    }
    encodingBuf[i] = '\0';
    s = encodingBuf;
  }
#else
  s = parser->m_protocolEncodingName;
#endif
  if ((parser->m_ns ? XmlInitEncodingNS : XmlInitEncoding)(
          &parser->m_initEncoding, &parser->m_encoding, s))
    return XML_ERROR_NONE;
  return handleUnknownEncoding(parser, parser->m_protocolEncodingName);
}