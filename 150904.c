xmlValidateAttributeCallback(xmlAttributePtr cur, xmlValidCtxtPtr ctxt,
	                    const xmlChar *name ATTRIBUTE_UNUSED) {
    int ret;
    xmlDocPtr doc;
    xmlElementPtr elem = NULL;

    if (cur == NULL)
	return;
    switch (cur->atype) {
	case XML_ATTRIBUTE_CDATA:
	case XML_ATTRIBUTE_ID:
	case XML_ATTRIBUTE_IDREF	:
	case XML_ATTRIBUTE_IDREFS:
	case XML_ATTRIBUTE_NMTOKEN:
	case XML_ATTRIBUTE_NMTOKENS:
	case XML_ATTRIBUTE_ENUMERATION:
	    break;
	case XML_ATTRIBUTE_ENTITY:
	case XML_ATTRIBUTE_ENTITIES:
	case XML_ATTRIBUTE_NOTATION:
	    if (cur->defaultValue != NULL) {

		ret = xmlValidateAttributeValue2(ctxt, ctxt->doc, cur->name,
			                         cur->atype, cur->defaultValue);
		if ((ret == 0) && (ctxt->valid == 1))
		    ctxt->valid = 0;
	    }
	    if (cur->tree != NULL) {
		xmlEnumerationPtr tree = cur->tree;
		while (tree != NULL) {
		    ret = xmlValidateAttributeValue2(ctxt, ctxt->doc,
				    cur->name, cur->atype, tree->name);
		    if ((ret == 0) && (ctxt->valid == 1))
			ctxt->valid = 0;
		    tree = tree->next;
		}
	    }
    }
    if (cur->atype == XML_ATTRIBUTE_NOTATION) {
	doc = cur->doc;
	if (cur->elem == NULL) {
	    xmlErrValid(ctxt, XML_ERR_INTERNAL_ERROR,
		   "xmlValidateAttributeCallback(%s): internal error\n",
		   (const char *) cur->name);
	    return;
	}

	if (doc != NULL)
	    elem = xmlGetDtdElementDesc(doc->intSubset, cur->elem);
	if ((elem == NULL) && (doc != NULL))
	    elem = xmlGetDtdElementDesc(doc->extSubset, cur->elem);
	if ((elem == NULL) && (cur->parent != NULL) &&
	    (cur->parent->type == XML_DTD_NODE))
	    elem = xmlGetDtdElementDesc((xmlDtdPtr) cur->parent, cur->elem);
	if (elem == NULL) {
	    xmlErrValidNode(ctxt, NULL, XML_DTD_UNKNOWN_ELEM,
		   "attribute %s: could not find decl for element %s\n",
		   cur->name, cur->elem, NULL);
	    return;
	}
	if (elem->etype == XML_ELEMENT_TYPE_EMPTY) {
	    xmlErrValidNode(ctxt, NULL, XML_DTD_EMPTY_NOTATION,
		   "NOTATION attribute %s declared for EMPTY element %s\n",
		   cur->name, cur->elem, NULL);
	    ctxt->valid = 0;
	}
    }
}