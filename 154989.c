xmlFreeParserCtxt(xmlParserCtxtPtr ctxt)
{
    xmlParserInputPtr input;

    if (ctxt == NULL) return;

    while ((input = inputPop(ctxt)) != NULL) { /* Non consuming */
        xmlFreeInputStream(input);
    }
    if (ctxt->spaceTab != NULL) xmlFree(ctxt->spaceTab);
    if (ctxt->nameTab != NULL) xmlFree((xmlChar * *)ctxt->nameTab);
    if (ctxt->nodeTab != NULL) xmlFree(ctxt->nodeTab);
    if (ctxt->nodeInfoTab != NULL) xmlFree(ctxt->nodeInfoTab);
    if (ctxt->inputTab != NULL) xmlFree(ctxt->inputTab);
    if (ctxt->version != NULL) xmlFree((char *) ctxt->version);
    if (ctxt->encoding != NULL) xmlFree((char *) ctxt->encoding);
    if (ctxt->extSubURI != NULL) xmlFree((char *) ctxt->extSubURI);
    if (ctxt->extSubSystem != NULL) xmlFree((char *) ctxt->extSubSystem);
#ifdef LIBXML_SAX1_ENABLED
    if ((ctxt->sax != NULL) &&
        (ctxt->sax != (xmlSAXHandlerPtr) &xmlDefaultSAXHandler))
#else
    if (ctxt->sax != NULL)
#endif /* LIBXML_SAX1_ENABLED */
        xmlFree(ctxt->sax);
    if (ctxt->directory != NULL) xmlFree((char *) ctxt->directory);
    if (ctxt->vctxt.nodeTab != NULL) xmlFree(ctxt->vctxt.nodeTab);
    if (ctxt->atts != NULL) xmlFree((xmlChar * *)ctxt->atts);
    if (ctxt->dict != NULL) xmlDictFree(ctxt->dict);
    if (ctxt->nsTab != NULL) xmlFree((char *) ctxt->nsTab);
    if (ctxt->pushTab != NULL) xmlFree(ctxt->pushTab);
    if (ctxt->attallocs != NULL) xmlFree(ctxt->attallocs);
    if (ctxt->attsDefault != NULL)
        xmlHashFree(ctxt->attsDefault, (xmlHashDeallocator) xmlFree);
    if (ctxt->attsSpecial != NULL)
        xmlHashFree(ctxt->attsSpecial, NULL);
    if (ctxt->freeElems != NULL) {
        xmlNodePtr cur, next;

	cur = ctxt->freeElems;
	while (cur != NULL) {
	    next = cur->next;
	    xmlFree(cur);
	    cur = next;
	}
    }
    if (ctxt->freeAttrs != NULL) {
        xmlAttrPtr cur, next;

	cur = ctxt->freeAttrs;
	while (cur != NULL) {
	    next = cur->next;
	    xmlFree(cur);
	    cur = next;
	}
    }
    /*
     * cleanup the error strings
     */
    if (ctxt->lastError.message != NULL)
        xmlFree(ctxt->lastError.message);
    if (ctxt->lastError.file != NULL)
        xmlFree(ctxt->lastError.file);
    if (ctxt->lastError.str1 != NULL)
        xmlFree(ctxt->lastError.str1);
    if (ctxt->lastError.str2 != NULL)
        xmlFree(ctxt->lastError.str2);
    if (ctxt->lastError.str3 != NULL)
        xmlFree(ctxt->lastError.str3);

#ifdef LIBXML_CATALOG_ENABLED
    if (ctxt->catalogs != NULL)
	xmlCatalogFreeLocal(ctxt->catalogs);
#endif
    xmlFree(ctxt);
}