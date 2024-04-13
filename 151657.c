void* Type_NamedColor_Dup(struct _cms_typehandler_struct* self, const void* Ptr, cmsUInt32Number n)
{
    cmsNAMEDCOLORLIST* nc = (cmsNAMEDCOLORLIST*) Ptr;

    return (void*) cmsDupNamedColorList(nc);

    cmsUNUSED_PARAMETER(n);
    cmsUNUSED_PARAMETER(self);
}