XML_ParserCreate_MM(const XML_Char *encodingName,
                    const XML_Memory_Handling_Suite *memsuite,
                    const XML_Char *nameSep) {
  return parserCreate(encodingName, memsuite, nameSep, NULL);
}