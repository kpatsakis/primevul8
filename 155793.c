XML_FreeContentModel(XML_Parser parser, XML_Content *model) {
  if (parser != NULL)
    FREE(parser, model);
}