nameNsPush(xmlParserCtxtPtr ctxt, const xmlChar * value,
           const xmlChar *prefix, const xmlChar *URI, int nsNr)
{
    if (ctxt->nameNr >= ctxt->nameMax) {
        const xmlChar * *tmp;
        void **tmp2;
        ctxt->nameMax *= 2;
        tmp = (const xmlChar * *) xmlRealloc((xmlChar * *)ctxt->nameTab,
                                    ctxt->nameMax *
                                    sizeof(ctxt->nameTab[0]));
        if (tmp == NULL) {
	    ctxt->nameMax /= 2;
	    goto mem_error;
        }
	ctxt->nameTab = tmp;
        tmp2 = (void **) xmlRealloc((void * *)ctxt->pushTab,
                                    ctxt->nameMax * 3 *
                                    sizeof(ctxt->pushTab[0]));
        if (tmp2 == NULL) {
	    ctxt->nameMax /= 2;
	    goto mem_error;
        }
	ctxt->pushTab = tmp2;
    } else if (ctxt->pushTab == NULL) {
        ctxt->pushTab = (void **) xmlMalloc(ctxt->nameMax * 3 *
                                            sizeof(ctxt->pushTab[0]));
        if (ctxt->pushTab == NULL)
            goto mem_error;
    }
    ctxt->nameTab[ctxt->nameNr] = value;
    ctxt->name = value;
    ctxt->pushTab[ctxt->nameNr * 3] = (void *) prefix;
    ctxt->pushTab[ctxt->nameNr * 3 + 1] = (void *) URI;
    ctxt->pushTab[ctxt->nameNr * 3 + 2] = (void *) (ptrdiff_t) nsNr;
    return (ctxt->nameNr++);
mem_error:
    xmlErrMemory(ctxt, NULL);
    return (-1);
}