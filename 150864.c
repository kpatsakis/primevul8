xmlValidGetValidElements(xmlNode *prev, xmlNode *next, const xmlChar **names,
                         int max) {
    xmlValidCtxt vctxt;
    int nb_valid_elements = 0;
    const xmlChar *elements[256]={0};
    int nb_elements = 0, i;
    const xmlChar *name;

    xmlNode *ref_node;
    xmlNode *parent;
    xmlNode *test_node;

    xmlNode *prev_next;
    xmlNode *next_prev;
    xmlNode *parent_childs;
    xmlNode *parent_last;

    xmlElement *element_desc;

    if (prev == NULL && next == NULL)
        return(-1);

    if (names == NULL) return(-1);
    if (max <= 0) return(-1);

    memset(&vctxt, 0, sizeof (xmlValidCtxt));
    vctxt.error = xmlNoValidityErr;	/* this suppresses err/warn output */

    nb_valid_elements = 0;
    ref_node = prev ? prev : next;
    parent = ref_node->parent;

    /*
     * Retrieves the parent element declaration
     */
    element_desc = xmlGetDtdElementDesc(parent->doc->intSubset,
                                         parent->name);
    if ((element_desc == NULL) && (parent->doc->extSubset != NULL))
        element_desc = xmlGetDtdElementDesc(parent->doc->extSubset,
                                             parent->name);
    if (element_desc == NULL) return(-1);

    /*
     * Do a backup of the current tree structure
     */
    prev_next = prev ? prev->next : NULL;
    next_prev = next ? next->prev : NULL;
    parent_childs = parent->children;
    parent_last = parent->last;

    /*
     * Creates a dummy node and insert it into the tree
     */
    test_node = xmlNewDocNode (ref_node->doc, NULL, BAD_CAST "<!dummy?>", NULL);
    if (test_node == NULL)
        return(-1);

    test_node->parent = parent;
    test_node->prev = prev;
    test_node->next = next;
    name = test_node->name;

    if (prev) prev->next = test_node;
    else parent->children = test_node;

    if (next) next->prev = test_node;
    else parent->last = test_node;

    /*
     * Insert each potential child node and check if the parent is
     * still valid
     */
    nb_elements = xmlValidGetPotentialChildren(element_desc->content,
		       elements, &nb_elements, 256);

    for (i = 0;i < nb_elements;i++) {
	test_node->name = elements[i];
	if (xmlValidateOneElement(&vctxt, parent->doc, parent)) {
	    int j;

	    for (j = 0; j < nb_valid_elements;j++)
		if (xmlStrEqual(elements[i], names[j])) break;
	    names[nb_valid_elements++] = elements[i];
	    if (nb_valid_elements >= max) break;
	}
    }

    /*
     * Restore the tree structure
     */
    if (prev) prev->next = prev_next;
    if (next) next->prev = next_prev;
    parent->children = parent_childs;
    parent->last = parent_last;

    /*
     * Free up the dummy node
     */
    test_node->name = name;
    xmlFreeNode(test_node);

    return(nb_valid_elements);
}