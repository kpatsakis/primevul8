xmlFreeInputStream(xmlParserInputPtr input) {
    if (input == NULL) return;

    if (input->filename != NULL) xmlFree((char *) input->filename);
    if (input->directory != NULL) xmlFree((char *) input->directory);
    if (input->encoding != NULL) xmlFree((char *) input->encoding);
    if (input->version != NULL) xmlFree((char *) input->version);
    if ((input->free != NULL) && (input->base != NULL))
        input->free((xmlChar *) input->base);
    if (input->buf != NULL)
        xmlFreeParserInputBuffer(input->buf);
    xmlFree(input);
}