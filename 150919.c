xmlValidateOneElement(xmlValidCtxtPtr ctxt, xmlDocPtr doc,
                      xmlNodePtr elem) {
    xmlElementPtr elemDecl = NULL;
    xmlElementContentPtr cont;
    xmlAttributePtr attr;
    xmlNodePtr child;
    int ret = 1, tmp;
    const xmlChar *name;
    int extsubset = 0;

    CHECK_DTD;

    if (elem == NULL) return(0);
    switch (elem->type) {
        case XML_ATTRIBUTE_NODE:
	    xmlErrValidNode(ctxt, elem, XML_ERR_INTERNAL_ERROR,
		   "Attribute element not expected\n", NULL, NULL ,NULL);
	    return(0);
        case XML_TEXT_NODE:
	    if (elem->children != NULL) {
		xmlErrValidNode(ctxt, elem, XML_ERR_INTERNAL_ERROR,
		                "Text element has children !\n",
				NULL,NULL,NULL);
		return(0);
	    }
	    if (elem->ns != NULL) {
		xmlErrValidNode(ctxt, elem, XML_ERR_INTERNAL_ERROR,
		                "Text element has namespace !\n",
				NULL,NULL,NULL);
		return(0);
	    }
	    if (elem->content == NULL) {
		xmlErrValidNode(ctxt, elem, XML_ERR_INTERNAL_ERROR,
		                "Text element has no content !\n",
				NULL,NULL,NULL);
		return(0);
	    }
	    return(1);
        case XML_XINCLUDE_START:
        case XML_XINCLUDE_END:
            return(1);
        case XML_CDATA_SECTION_NODE:
        case XML_ENTITY_REF_NODE:
        case XML_PI_NODE:
        case XML_COMMENT_NODE:
	    return(1);
        case XML_ENTITY_NODE:
	    xmlErrValidNode(ctxt, elem, XML_ERR_INTERNAL_ERROR,
		   "Entity element not expected\n", NULL, NULL ,NULL);
	    return(0);
        case XML_NOTATION_NODE:
	    xmlErrValidNode(ctxt, elem, XML_ERR_INTERNAL_ERROR,
		   "Notation element not expected\n", NULL, NULL ,NULL);
	    return(0);
        case XML_DOCUMENT_NODE:
        case XML_DOCUMENT_TYPE_NODE:
        case XML_DOCUMENT_FRAG_NODE:
	    xmlErrValidNode(ctxt, elem, XML_ERR_INTERNAL_ERROR,
		   "Document element not expected\n", NULL, NULL ,NULL);
	    return(0);
        case XML_HTML_DOCUMENT_NODE:
	    xmlErrValidNode(ctxt, elem, XML_ERR_INTERNAL_ERROR,
		   "HTML Document not expected\n", NULL, NULL ,NULL);
	    return(0);
        case XML_ELEMENT_NODE:
	    break;
	default:
	    xmlErrValidNode(ctxt, elem, XML_ERR_INTERNAL_ERROR,
		   "unknown element type\n", NULL, NULL ,NULL);
	    return(0);
    }

    /*
     * Fetch the declaration
     */
    elemDecl = xmlValidGetElemDecl(ctxt, doc, elem, &extsubset);
    if (elemDecl == NULL)
	return(0);

    /*
     * If vstateNr is not zero that means continuous validation is
     * activated, do not try to check the content model at that level.
     */
    if (ctxt->vstateNr == 0) {
    /* Check that the element content matches the definition */
    switch (elemDecl->etype) {
        case XML_ELEMENT_TYPE_UNDEFINED:
	    xmlErrValidNode(ctxt, elem, XML_DTD_UNKNOWN_ELEM,
	                    "No declaration for element %s\n",
		   elem->name, NULL, NULL);
	    return(0);
        case XML_ELEMENT_TYPE_EMPTY:
	    if (elem->children != NULL) {
		xmlErrValidNode(ctxt, elem, XML_DTD_NOT_EMPTY,
	       "Element %s was declared EMPTY this one has content\n",
	               elem->name, NULL, NULL);
		ret = 0;
	    }
	    break;
        case XML_ELEMENT_TYPE_ANY:
	    /* I don't think anything is required then */
	    break;
        case XML_ELEMENT_TYPE_MIXED:

	    /* simple case of declared as #PCDATA */
	    if ((elemDecl->content != NULL) &&
		(elemDecl->content->type == XML_ELEMENT_CONTENT_PCDATA)) {
		ret = xmlValidateOneCdataElement(ctxt, doc, elem);
		if (!ret) {
		    xmlErrValidNode(ctxt, elem, XML_DTD_NOT_PCDATA,
	       "Element %s was declared #PCDATA but contains non text nodes\n",
			   elem->name, NULL, NULL);
		}
		break;
	    }
	    child = elem->children;
	    /* Hum, this start to get messy */
	    while (child != NULL) {
	        if (child->type == XML_ELEMENT_NODE) {
		    name = child->name;
		    if ((child->ns != NULL) && (child->ns->prefix != NULL)) {
			xmlChar fn[50];
			xmlChar *fullname;

			fullname = xmlBuildQName(child->name, child->ns->prefix,
				                 fn, 50);
			if (fullname == NULL)
			    return(0);
			cont = elemDecl->content;
			while (cont != NULL) {
			    if (cont->type == XML_ELEMENT_CONTENT_ELEMENT) {
				if (xmlStrEqual(cont->name, fullname))
				    break;
			    } else if ((cont->type == XML_ELEMENT_CONTENT_OR) &&
			       (cont->c1 != NULL) &&
			       (cont->c1->type == XML_ELEMENT_CONTENT_ELEMENT)){
				if (xmlStrEqual(cont->c1->name, fullname))
				    break;
			    } else if ((cont->type != XML_ELEMENT_CONTENT_OR) ||
				(cont->c1 == NULL) ||
				(cont->c1->type != XML_ELEMENT_CONTENT_PCDATA)){
				xmlErrValid(NULL, XML_DTD_MIXED_CORRUPT,
					"Internal: MIXED struct corrupted\n",
					NULL);
				break;
			    }
			    cont = cont->c2;
			}
			if ((fullname != fn) && (fullname != child->name))
			    xmlFree(fullname);
			if (cont != NULL)
			    goto child_ok;
		    }
		    cont = elemDecl->content;
		    while (cont != NULL) {
		        if (cont->type == XML_ELEMENT_CONTENT_ELEMENT) {
			    if (xmlStrEqual(cont->name, name)) break;
			} else if ((cont->type == XML_ELEMENT_CONTENT_OR) &&
			   (cont->c1 != NULL) &&
			   (cont->c1->type == XML_ELEMENT_CONTENT_ELEMENT)) {
			    if (xmlStrEqual(cont->c1->name, name)) break;
			} else if ((cont->type != XML_ELEMENT_CONTENT_OR) ||
			    (cont->c1 == NULL) ||
			    (cont->c1->type != XML_ELEMENT_CONTENT_PCDATA)) {
			    xmlErrValid(ctxt, XML_DTD_MIXED_CORRUPT,
				    "Internal: MIXED struct corrupted\n",
				    NULL);
			    break;
			}
			cont = cont->c2;
		    }
		    if (cont == NULL) {
			xmlErrValidNode(ctxt, elem, XML_DTD_INVALID_CHILD,
	       "Element %s is not declared in %s list of possible children\n",
			       name, elem->name, NULL);
			ret = 0;
		    }
		}
child_ok:
	        child = child->next;
	    }
	    break;
        case XML_ELEMENT_TYPE_ELEMENT:
	    if ((doc->standalone == 1) && (extsubset == 1)) {
		/*
		 * VC: Standalone Document Declaration
		 *     - element types with element content, if white space
		 *       occurs directly within any instance of those types.
		 */
		child = elem->children;
		while (child != NULL) {
		    if (child->type == XML_TEXT_NODE) {
			const xmlChar *content = child->content;

			while (IS_BLANK_CH(*content))
			    content++;
			if (*content == 0) {
			    xmlErrValidNode(ctxt, elem,
			                    XML_DTD_STANDALONE_WHITE_SPACE,
"standalone: %s declared in the external subset contains white spaces nodes\n",
				   elem->name, NULL, NULL);
			    ret = 0;
			    break;
			}
		    }
		    child =child->next;
		}
	    }
	    child = elem->children;
	    cont = elemDecl->content;
	    tmp = xmlValidateElementContent(ctxt, child, elemDecl, 1, elem);
	    if (tmp <= 0)
		ret = tmp;
	    break;
    }
    } /* not continuous */

    /* [ VC: Required Attribute ] */
    attr = elemDecl->attributes;
    while (attr != NULL) {
	if (attr->def == XML_ATTRIBUTE_REQUIRED) {
	    int qualified = -1;

	    if ((attr->prefix == NULL) &&
		(xmlStrEqual(attr->name, BAD_CAST "xmlns"))) {
		xmlNsPtr ns;

		ns = elem->nsDef;
		while (ns != NULL) {
		    if (ns->prefix == NULL)
			goto found;
		    ns = ns->next;
		}
	    } else if (xmlStrEqual(attr->prefix, BAD_CAST "xmlns")) {
		xmlNsPtr ns;

		ns = elem->nsDef;
		while (ns != NULL) {
		    if (xmlStrEqual(attr->name, ns->prefix))
			goto found;
		    ns = ns->next;
		}
	    } else {
		xmlAttrPtr attrib;

		attrib = elem->properties;
		while (attrib != NULL) {
		    if (xmlStrEqual(attrib->name, attr->name)) {
			if (attr->prefix != NULL) {
			    xmlNsPtr nameSpace = attrib->ns;

			    if (nameSpace == NULL)
				nameSpace = elem->ns;
			    /*
			     * qualified names handling is problematic, having a
			     * different prefix should be possible but DTDs don't
			     * allow to define the URI instead of the prefix :-(
			     */
			    if (nameSpace == NULL) {
				if (qualified < 0)
				    qualified = 0;
			    } else if (!xmlStrEqual(nameSpace->prefix,
						    attr->prefix)) {
				if (qualified < 1)
				    qualified = 1;
			    } else
				goto found;
			} else {
			    /*
			     * We should allow applications to define namespaces
			     * for their application even if the DTD doesn't
			     * carry one, otherwise, basically we would always
			     * break.
			     */
			    goto found;
			}
		    }
		    attrib = attrib->next;
		}
	    }
	    if (qualified == -1) {
		if (attr->prefix == NULL) {
		    xmlErrValidNode(ctxt, elem, XML_DTD_MISSING_ATTRIBUTE,
		       "Element %s does not carry attribute %s\n",
			   elem->name, attr->name, NULL);
		    ret = 0;
	        } else {
		    xmlErrValidNode(ctxt, elem, XML_DTD_MISSING_ATTRIBUTE,
		       "Element %s does not carry attribute %s:%s\n",
			   elem->name, attr->prefix,attr->name);
		    ret = 0;
		}
	    } else if (qualified == 0) {
		xmlErrValidWarning(ctxt, elem, XML_DTD_NO_PREFIX,
		   "Element %s required attribute %s:%s has no prefix\n",
		       elem->name, attr->prefix, attr->name);
	    } else if (qualified == 1) {
		xmlErrValidWarning(ctxt, elem, XML_DTD_DIFFERENT_PREFIX,
		   "Element %s required attribute %s:%s has different prefix\n",
		       elem->name, attr->prefix, attr->name);
	    }
	} else if (attr->def == XML_ATTRIBUTE_FIXED) {
	    /*
	     * Special tests checking #FIXED namespace declarations
	     * have the right value since this is not done as an
	     * attribute checking
	     */
	    if ((attr->prefix == NULL) &&
		(xmlStrEqual(attr->name, BAD_CAST "xmlns"))) {
		xmlNsPtr ns;

		ns = elem->nsDef;
		while (ns != NULL) {
		    if (ns->prefix == NULL) {
			if (!xmlStrEqual(attr->defaultValue, ns->href)) {
			    xmlErrValidNode(ctxt, elem,
			           XML_DTD_ELEM_DEFAULT_NAMESPACE,
   "Element %s namespace name for default namespace does not match the DTD\n",
				   elem->name, NULL, NULL);
			    ret = 0;
			}
			goto found;
		    }
		    ns = ns->next;
		}
	    } else if (xmlStrEqual(attr->prefix, BAD_CAST "xmlns")) {
		xmlNsPtr ns;

		ns = elem->nsDef;
		while (ns != NULL) {
		    if (xmlStrEqual(attr->name, ns->prefix)) {
			if (!xmlStrEqual(attr->defaultValue, ns->href)) {
			    xmlErrValidNode(ctxt, elem, XML_DTD_ELEM_NAMESPACE,
		   "Element %s namespace name for %s does not match the DTD\n",
				   elem->name, ns->prefix, NULL);
			    ret = 0;
			}
			goto found;
		    }
		    ns = ns->next;
		}
	    }
	}
found:
        attr = attr->nexth;
    }
    return(ret);
}