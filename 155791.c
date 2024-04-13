XML_ParserCreate(const XML_Char *encodingName) {
  return XML_ParserCreate_MM(encodingName, NULL, NULL);
}