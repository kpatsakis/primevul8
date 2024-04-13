cmsBool  Type_Chromaticity_Write(struct _cms_typehandler_struct* self, cmsIOHANDLER* io, void* Ptr, cmsUInt32Number nItems)
{
    cmsCIExyYTRIPLE* chrm = (cmsCIExyYTRIPLE*) Ptr;

    if (!_cmsWriteUInt16Number(io, 3)) return FALSE;        // nChannels
    if (!_cmsWriteUInt16Number(io, 0)) return FALSE;        // Table

    if (!SaveOneChromaticity(chrm -> Red.x,   chrm -> Red.y, io)) return FALSE;
    if (!SaveOneChromaticity(chrm -> Green.x, chrm -> Green.y, io)) return FALSE;
    if (!SaveOneChromaticity(chrm -> Blue.x,  chrm -> Blue.y, io)) return FALSE;

    return TRUE;

    cmsUNUSED_PARAMETER(nItems);
    cmsUNUSED_PARAMETER(self);
}