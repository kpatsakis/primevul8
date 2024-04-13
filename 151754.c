void *Type_CrdInfo_Read(struct _cms_typehandler_struct* self, cmsIOHANDLER* io, cmsUInt32Number* nItems, cmsUInt32Number SizeOfTag)
{
    cmsMLU* mlu = cmsMLUalloc(self ->ContextID, 5);

    *nItems = 0;
    if (!ReadCountAndSting(self, io, mlu, &SizeOfTag, "nm")) goto Error;
    if (!ReadCountAndSting(self, io, mlu, &SizeOfTag, "#0")) goto Error;
    if (!ReadCountAndSting(self, io, mlu, &SizeOfTag, "#1")) goto Error;
    if (!ReadCountAndSting(self, io, mlu, &SizeOfTag, "#2")) goto Error;
    if (!ReadCountAndSting(self, io, mlu, &SizeOfTag, "#3")) goto Error;

    *nItems = 1;
    return (void*) mlu;

Error:
    cmsMLUfree(mlu);
    return NULL;

}