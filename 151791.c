void *Type_Text_Read(struct _cms_typehandler_struct* self, cmsIOHANDLER* io, cmsUInt32Number* nItems, cmsUInt32Number SizeOfTag)
{
    char* Text = NULL;
    cmsMLU* mlu = NULL;

    // Create a container
    mlu = cmsMLUalloc(self ->ContextID, 1);
    if (mlu == NULL) return NULL;

    *nItems = 0;

    // We need to store the "\0" at the end, so +1
    if (SizeOfTag == UINT_MAX) goto Error;

    Text = (char*) _cmsMalloc(self ->ContextID, SizeOfTag + 1);
    if (Text == NULL) goto Error;

    if (io -> Read(io, Text, sizeof(char), SizeOfTag) != SizeOfTag) goto Error;

    // Make sure text is properly ended
    Text[SizeOfTag] = 0;
    *nItems = 1;

    // Keep the result
    if (!cmsMLUsetASCII(mlu, cmsNoLanguage, cmsNoCountry, Text)) goto Error;

    _cmsFree(self ->ContextID, Text);
    return (void*) mlu;

Error:
    if (mlu != NULL)
        cmsMLUfree(mlu);
    if (Text != NULL)
        _cmsFree(self ->ContextID, Text);

    return NULL;
}