xmlWalkRemoveRef(const void *data, const void *user)
{
    xmlAttrPtr attr0 = ((xmlRefPtr)data)->attr;
    xmlAttrPtr attr1 = ((xmlRemoveMemoPtr)user)->ap;
    xmlListPtr ref_list = ((xmlRemoveMemoPtr)user)->l;

    if (attr0 == attr1) { /* Matched: remove and terminate walk */
        xmlListRemoveFirst(ref_list, (void *)data);
        return 0;
    }
    return 1;
}