xmlValidateElement(xmlValidCtxtPtr ctxt, xmlDocPtr doc, xmlNodePtr elem) {
    xmlNodePtr child;
    xmlAttrPtr attr;
    xmlNsPtr ns;
    const xmlChar *value;
    int ret = 1;

    if (elem == NULL) return(0);

    /*
     * XInclude elements were added after parsing in the infoset,
     * they don't really mean anything validation wise.
     */
    if ((elem->type == XML_XINCLUDE_START) ||
	(elem->type == XML_XINCLUDE_END) ||
	(elem->type == XML_NAMESPACE_DECL))
	return(1);

    CHECK_DTD;

    /*
     * Entities references have to be handled separately
     */
    if (elem->type == XML_ENTITY_REF_NODE) {
	return(1);
    }

    ret &= xmlValidateOneElement(ctxt, doc, elem);
    if (elem->type == XML_ELEMENT_NODE) {
	attr = elem->properties;
	while (attr != NULL) {
	    value = xmlNodeListGetString(doc, attr->children, 0);
	    ret &= xmlValidateOneAttribute(ctxt, doc, elem, attr, value);
	    if (value != NULL)
		xmlFree((char *)value);
	    attr= attr->next;
	}
	ns = elem->nsDef;
	while (ns != NULL) {
	    if (elem->ns == NULL)
		ret &= xmlValidateOneNamespace(ctxt, doc, elem, NULL,
					       ns, ns->href);
	    else
		ret &= xmlValidateOneNamespace(ctxt, doc, elem,
		                               elem->ns->prefix, ns, ns->href);
	    ns = ns->next;
	}
    }
    child = elem->children;
    while (child != NULL) {
        ret &= xmlValidateElement(ctxt, doc, child);
        child = child->next;
    }

    return(ret);
}