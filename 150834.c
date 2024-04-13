xmlValidBuildAContentModel(xmlElementContentPtr content,
		           xmlValidCtxtPtr ctxt,
		           const xmlChar *name) {
    if (content == NULL) {
	xmlErrValidNode(ctxt, NULL, XML_ERR_INTERNAL_ERROR,
			"Found NULL content in content model of %s\n",
			name, NULL, NULL);
	return(0);
    }
    switch (content->type) {
	case XML_ELEMENT_CONTENT_PCDATA:
	    xmlErrValidNode(ctxt, NULL, XML_ERR_INTERNAL_ERROR,
			    "Found PCDATA in content model of %s\n",
		            name, NULL, NULL);
	    return(0);
	    break;
	case XML_ELEMENT_CONTENT_ELEMENT: {
	    xmlAutomataStatePtr oldstate = ctxt->state;
	    xmlChar fn[50];
	    xmlChar *fullname;

	    fullname = xmlBuildQName(content->name, content->prefix, fn, 50);
	    if (fullname == NULL) {
	        xmlVErrMemory(ctxt, "Building content model");
		return(0);
	    }

	    switch (content->ocur) {
		case XML_ELEMENT_CONTENT_ONCE:
		    ctxt->state = xmlAutomataNewTransition(ctxt->am,
			    ctxt->state, NULL, fullname, NULL);
		    break;
		case XML_ELEMENT_CONTENT_OPT:
		    ctxt->state = xmlAutomataNewTransition(ctxt->am,
			    ctxt->state, NULL, fullname, NULL);
		    xmlAutomataNewEpsilon(ctxt->am, oldstate, ctxt->state);
		    break;
		case XML_ELEMENT_CONTENT_PLUS:
		    ctxt->state = xmlAutomataNewTransition(ctxt->am,
			    ctxt->state, NULL, fullname, NULL);
		    xmlAutomataNewTransition(ctxt->am, ctxt->state,
			                     ctxt->state, fullname, NULL);
		    break;
		case XML_ELEMENT_CONTENT_MULT:
		    ctxt->state = xmlAutomataNewEpsilon(ctxt->am,
					    ctxt->state, NULL);
		    xmlAutomataNewTransition(ctxt->am,
			    ctxt->state, ctxt->state, fullname, NULL);
		    break;
	    }
	    if ((fullname != fn) && (fullname != content->name))
		xmlFree(fullname);
	    break;
	}
	case XML_ELEMENT_CONTENT_SEQ: {
	    xmlAutomataStatePtr oldstate, oldend;
	    xmlElementContentOccur ocur;

	    /*
	     * Simply iterate over the content
	     */
	    oldstate = ctxt->state;
	    ocur = content->ocur;
	    if (ocur != XML_ELEMENT_CONTENT_ONCE) {
		ctxt->state = xmlAutomataNewEpsilon(ctxt->am, oldstate, NULL);
		oldstate = ctxt->state;
	    }
	    do {
		xmlValidBuildAContentModel(content->c1, ctxt, name);
		content = content->c2;
	    } while ((content->type == XML_ELEMENT_CONTENT_SEQ) &&
		     (content->ocur == XML_ELEMENT_CONTENT_ONCE));
	    xmlValidBuildAContentModel(content, ctxt, name);
	    oldend = ctxt->state;
	    ctxt->state = xmlAutomataNewEpsilon(ctxt->am, oldend, NULL);
	    switch (ocur) {
		case XML_ELEMENT_CONTENT_ONCE:
		    break;
		case XML_ELEMENT_CONTENT_OPT:
		    xmlAutomataNewEpsilon(ctxt->am, oldstate, ctxt->state);
		    break;
		case XML_ELEMENT_CONTENT_MULT:
		    xmlAutomataNewEpsilon(ctxt->am, oldstate, ctxt->state);
		    xmlAutomataNewEpsilon(ctxt->am, oldend, oldstate);
		    break;
		case XML_ELEMENT_CONTENT_PLUS:
		    xmlAutomataNewEpsilon(ctxt->am, oldend, oldstate);
		    break;
	    }
	    break;
	}
	case XML_ELEMENT_CONTENT_OR: {
	    xmlAutomataStatePtr oldstate, oldend;
	    xmlElementContentOccur ocur;

	    ocur = content->ocur;
	    if ((ocur == XML_ELEMENT_CONTENT_PLUS) ||
		(ocur == XML_ELEMENT_CONTENT_MULT)) {
		ctxt->state = xmlAutomataNewEpsilon(ctxt->am,
			ctxt->state, NULL);
	    }
	    oldstate = ctxt->state;
	    oldend = xmlAutomataNewState(ctxt->am);

	    /*
	     * iterate over the subtypes and remerge the end with an
	     * epsilon transition
	     */
	    do {
		ctxt->state = oldstate;
		xmlValidBuildAContentModel(content->c1, ctxt, name);
		xmlAutomataNewEpsilon(ctxt->am, ctxt->state, oldend);
		content = content->c2;
	    } while ((content->type == XML_ELEMENT_CONTENT_OR) &&
		     (content->ocur == XML_ELEMENT_CONTENT_ONCE));
	    ctxt->state = oldstate;
	    xmlValidBuildAContentModel(content, ctxt, name);
	    xmlAutomataNewEpsilon(ctxt->am, ctxt->state, oldend);
	    ctxt->state = xmlAutomataNewEpsilon(ctxt->am, oldend, NULL);
	    switch (ocur) {
		case XML_ELEMENT_CONTENT_ONCE:
		    break;
		case XML_ELEMENT_CONTENT_OPT:
		    xmlAutomataNewEpsilon(ctxt->am, oldstate, ctxt->state);
		    break;
		case XML_ELEMENT_CONTENT_MULT:
		    xmlAutomataNewEpsilon(ctxt->am, oldstate, ctxt->state);
		    xmlAutomataNewEpsilon(ctxt->am, oldend, oldstate);
		    break;
		case XML_ELEMENT_CONTENT_PLUS:
		    xmlAutomataNewEpsilon(ctxt->am, oldend, oldstate);
		    break;
	    }
	    break;
	}
	default:
	    xmlErrValid(ctxt, XML_ERR_INTERNAL_ERROR,
	                "ContentModel broken for element %s\n",
			(const char *) name);
	    return(0);
    }
    return(1);
}