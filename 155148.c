Bool TY_(AddGenerator)( TidyDocImpl* doc )
{
    AttVal *attval;
    Node *node;
    Node *head = TY_(FindHEAD)( doc );
    tmbchar buf[256];
    
    if (head)
    {
#ifdef PLATFORM_NAME
        TY_(tmbsnprintf)(buf, sizeof(buf), "HTML Tidy for HTML5 for "PLATFORM_NAME" version %s",
                         tidyLibraryVersion());
#else
        TY_(tmbsnprintf)(buf, sizeof(buf), "HTML Tidy for HTML5 version %s", tidyLibraryVersion());
#endif

        for ( node = head->content; node; node = node->next )
        {
            if ( nodeIsMETA(node) )
            {
                attval = TY_(AttrGetById)(node, TidyAttr_NAME);

                if (AttrValueIs(attval, "generator"))
                {
                    attval = TY_(AttrGetById)(node, TidyAttr_CONTENT);

                    if (AttrHasValue(attval) &&
                        TY_(tmbstrncasecmp)(attval->value, "HTML Tidy", 9) == 0)
                    {
                        /* update the existing content to reflect the */
                        /* actual version of Tidy currently being used */
                        
                        TidyDocFree(doc, attval->value);
                        attval->value = TY_(tmbstrdup)(doc->allocator, buf);
                        return no;
                    }
                }
            }
        }

        if ( cfg(doc, TidyAccessibilityCheckLevel) == 0 )
        {
            node = TY_(InferredTag)(doc, TidyTag_META);
            TY_(AddAttribute)( doc, node, "name", "generator" );
            TY_(AddAttribute)( doc, node, "content", buf );
            TY_(InsertNodeAtStart)( head, node );
            return yes;
        }
    }

    return no;
}