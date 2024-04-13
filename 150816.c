xmlValidCtxtPtr xmlNewValidCtxt(void) {
    xmlValidCtxtPtr ret;

    if ((ret = xmlMalloc(sizeof (xmlValidCtxt))) == NULL) {
	xmlVErrMemory(NULL, "malloc failed");
	return (NULL);
    }

    (void) memset(ret, 0, sizeof (xmlValidCtxt));

    return (ret);
}