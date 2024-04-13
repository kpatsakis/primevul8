void *Type_Chromaticity_Read(struct _cms_typehandler_struct* self, cmsIOHANDLER* io, cmsUInt32Number* nItems, cmsUInt32Number SizeOfTag)
{
    cmsCIExyYTRIPLE* chrm;
    cmsUInt16Number nChans, Table;

    *nItems = 0;
    chrm =  (cmsCIExyYTRIPLE*) _cmsMallocZero(self ->ContextID, sizeof(cmsCIExyYTRIPLE));
    if (chrm == NULL) return NULL;

    if (!_cmsReadUInt16Number(io, &nChans)) goto Error;

    // Let's recover from a bug introduced in early versions of lcms1
    if (nChans == 0 && SizeOfTag == 32) {

        if (!_cmsReadUInt16Number(io, NULL)) goto Error;
        if (!_cmsReadUInt16Number(io, &nChans)) goto Error;
    }

    if (nChans != 3) goto Error;

    if (!_cmsReadUInt16Number(io, &Table)) goto Error;

    if (!_cmsRead15Fixed16Number(io, &chrm ->Red.x)) goto Error;
    if (!_cmsRead15Fixed16Number(io, &chrm ->Red.y)) goto Error;

    chrm ->Red.Y = 1.0;

    if (!_cmsRead15Fixed16Number(io, &chrm ->Green.x)) goto Error;
    if (!_cmsRead15Fixed16Number(io, &chrm ->Green.y)) goto Error;

    chrm ->Green.Y = 1.0;

    if (!_cmsRead15Fixed16Number(io, &chrm ->Blue.x)) goto Error;
    if (!_cmsRead15Fixed16Number(io, &chrm ->Blue.y)) goto Error;

    chrm ->Blue.Y = 1.0;

    *nItems = 1;
    return (void*) chrm;

Error:
    _cmsFree(self ->ContextID, (void*) chrm);
    return NULL;

    cmsUNUSED_PARAMETER(SizeOfTag);
}