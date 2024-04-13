cmsBool  Type_CrdInfo_Write(struct _cms_typehandler_struct* self, cmsIOHANDLER* io, void* Ptr, cmsUInt32Number nItems)
{

    cmsMLU* mlu = (cmsMLU*) Ptr;

    if (!WriteCountAndSting(self, io, mlu, "nm")) goto Error;
    if (!WriteCountAndSting(self, io, mlu, "#0")) goto Error;
    if (!WriteCountAndSting(self, io, mlu, "#1")) goto Error;
    if (!WriteCountAndSting(self, io, mlu, "#2")) goto Error;
    if (!WriteCountAndSting(self, io, mlu, "#3")) goto Error;

    return TRUE;

Error:
    return FALSE;

    cmsUNUSED_PARAMETER(nItems);
}