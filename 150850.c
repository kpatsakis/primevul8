xmlValidateRoot(xmlValidCtxtPtr ctxt, xmlDocPtr doc) {
    xmlNodePtr root;
    int ret;

    if (doc == NULL) return(0);

    root = xmlDocGetRootElement(doc);
    if ((root == NULL) || (root->name == NULL)) {
	xmlErrValid(ctxt, XML_DTD_NO_ROOT,
	            "no root element\n", NULL);
        return(0);
    }

    /*
     * When doing post validation against a separate DTD, those may
     * no internal subset has been generated
     */
    if ((doc->intSubset != NULL) &&
	(doc->intSubset->name != NULL)) {
	/*
	 * Check first the document root against the NQName
	 */
	if (!xmlStrEqual(doc->intSubset->name, root->name)) {
	    if ((root->ns != NULL) && (root->ns->prefix != NULL)) {
		xmlChar fn[50];
		xmlChar *fullname;

		fullname = xmlBuildQName(root->name, root->ns->prefix, fn, 50);
		if (fullname == NULL) {
		    xmlVErrMemory(ctxt, NULL);
		    return(0);
		}
		ret = xmlStrEqual(doc->intSubset->name, fullname);
		if ((fullname != fn) && (fullname != root->name))
		    xmlFree(fullname);
		if (ret == 1)
		    goto name_ok;
	    }
	    if ((xmlStrEqual(doc->intSubset->name, BAD_CAST "HTML")) &&
		(xmlStrEqual(root->name, BAD_CAST "html")))
		goto name_ok;
	    xmlErrValidNode(ctxt, root, XML_DTD_ROOT_NAME,
		   "root and DTD name do not match '%s' and '%s'\n",
		   root->name, doc->intSubset->name, NULL);
	    return(0);
	}
    }
name_ok:
    return(1);
}