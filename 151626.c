cmsBool  Type_ColorantTable_Write(struct _cms_typehandler_struct* self, cmsIOHANDLER* io, void* Ptr, cmsUInt32Number nItems)
{
    cmsNAMEDCOLORLIST* NamedColorList = (cmsNAMEDCOLORLIST*) Ptr;
    int i, nColors;

    nColors = cmsNamedColorCount(NamedColorList);

    if (!_cmsWriteUInt32Number(io, nColors)) return FALSE;

    for (i=0; i < nColors; i++) {

        char root[33];
        cmsUInt16Number PCS[3];

        memset(root, 0, sizeof(root));

        if (!cmsNamedColorInfo(NamedColorList, i, root, NULL, NULL, PCS, NULL)) return 0;
        root[32] = 0;

        if (!io ->Write(io, 32, root)) return FALSE;
        if (!_cmsWriteUInt16Array(io, 3, PCS)) return FALSE;
    }

    return TRUE;

    cmsUNUSED_PARAMETER(nItems);
    cmsUNUSED_PARAMETER(self);
}