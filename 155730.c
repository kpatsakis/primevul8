XML_ParserCreateNS(const XML_Char *encodingName, XML_Char nsSep) {
  XML_Char tmp[2];
  *tmp = nsSep;
  return XML_ParserCreate_MM(encodingName, NULL, tmp);
}