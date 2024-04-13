xmlRemoveRef(xmlDocPtr doc, xmlAttrPtr attr) {
    xmlListPtr ref_list;
    xmlRefTablePtr table;
    xmlChar *ID;
    xmlRemoveMemo target;

    if (doc == NULL) return(-1);
    if (attr == NULL) return(-1);

    table = (xmlRefTablePtr) doc->refs;
    if (table == NULL)
        return(-1);

    ID = xmlNodeListGetString(doc, attr->children, 1);
    if (ID == NULL)
        return(-1);

    ref_list = xmlHashLookup(table, ID);
    if(ref_list == NULL) {
        xmlFree(ID);
        return (-1);
    }

    /* At this point, ref_list refers to a list of references which
     * have the same key as the supplied attr. Our list of references
     * is ordered by reference address and we don't have that information
     * here to use when removing. We'll have to walk the list and
     * check for a matching attribute, when we find one stop the walk
     * and remove the entry.
     * The list is ordered by reference, so that means we don't have the
     * key. Passing the list and the reference to the walker means we
     * will have enough data to be able to remove the entry.
     */
    target.l = ref_list;
    target.ap = attr;

    /* Remove the supplied attr from our list */
    xmlListWalk(ref_list, xmlWalkRemoveRef, &target);

    /*If the list is empty then remove the list entry in the hash */
    if (xmlListEmpty(ref_list))
        xmlHashUpdateEntry(table, ID, NULL, (xmlHashDeallocator)
        xmlFreeRefList);
    xmlFree(ID);
    return(0);
}