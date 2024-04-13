cmsBool WriteCLUT(struct _cms_typehandler_struct* self, cmsIOHANDLER* io, cmsUInt8Number  Precision, cmsStage* mpe)
{
    cmsUInt8Number  gridPoints[cmsMAXCHANNELS]; // Number of grid points in each dimension.
    cmsUInt32Number i;
    _cmsStageCLutData* CLUT = ( _cmsStageCLutData*) mpe -> Data;

    if (CLUT ->HasFloatValues) {
         cmsSignalError(self ->ContextID, cmsERROR_NOT_SUITABLE, "Cannot save floating point data, CLUT are 8 or 16 bit only");
         return FALSE;
    }

    memset(gridPoints, 0, sizeof(gridPoints));
    for (i=0; i < (cmsUInt32Number) CLUT ->Params ->nInputs; i++)
        gridPoints[i] = (cmsUInt8Number) CLUT ->Params ->nSamples[i];

    if (!io -> Write(io, cmsMAXCHANNELS*sizeof(cmsUInt8Number), gridPoints)) return FALSE;

    if (!_cmsWriteUInt8Number(io, (cmsUInt8Number) Precision)) return FALSE;
    if (!_cmsWriteUInt8Number(io, 0)) return FALSE;
    if (!_cmsWriteUInt8Number(io, 0)) return FALSE;
    if (!_cmsWriteUInt8Number(io, 0)) return FALSE;

    // Precision can be 1 or 2 bytes
    if (Precision == 1) {

        for (i=0; i < CLUT->nEntries; i++) {

            if (!_cmsWriteUInt8Number(io, FROM_16_TO_8(CLUT->Tab.T[i]))) return FALSE;
        }
    }
    else
        if (Precision == 2) {

            if (!_cmsWriteUInt16Array(io, CLUT->nEntries, CLUT ->Tab.T)) return FALSE;
        }
        else {
             cmsSignalError(self ->ContextID, cmsERROR_UNKNOWN_EXTENSION, "Unknown precision of '%d'", Precision);
            return FALSE;
        }

        if (!_cmsWriteAlignment(io)) return FALSE;

        return TRUE;
}