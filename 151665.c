cmsBool  Type_LUT8_Write(struct _cms_typehandler_struct* self, cmsIOHANDLER* io, void* Ptr, cmsUInt32Number nItems)
{
    cmsUInt32Number j, nTabSize;
    cmsUInt8Number  val;
    cmsPipeline* NewLUT = (cmsPipeline*) Ptr;
    cmsStage* mpe;
    _cmsStageToneCurvesData* PreMPE = NULL, *PostMPE = NULL;
    _cmsStageMatrixData* MatMPE = NULL;
    _cmsStageCLutData* clut = NULL;
    int clutPoints;

    // Disassemble the LUT into components.
    mpe = NewLUT -> Elements;
    if (mpe ->Type == cmsSigMatrixElemType) {

        MatMPE = (_cmsStageMatrixData*) mpe ->Data;
        mpe = mpe -> Next;
    }

    if (mpe != NULL && mpe ->Type == cmsSigCurveSetElemType) {
        PreMPE = (_cmsStageToneCurvesData*) mpe ->Data;
        mpe = mpe -> Next;
    }

    if (mpe != NULL && mpe ->Type == cmsSigCLutElemType) {
        clut  = (_cmsStageCLutData*) mpe -> Data;
        mpe = mpe ->Next;
    }

    if (mpe != NULL && mpe ->Type == cmsSigCurveSetElemType) {
        PostMPE = (_cmsStageToneCurvesData*) mpe ->Data;
        mpe = mpe -> Next;
    }

    // That should be all
    if (mpe != NULL) {
        cmsSignalError(mpe->ContextID, cmsERROR_UNKNOWN_EXTENSION, "LUT is not suitable to be saved as LUT8");
        return FALSE;
    }


    if (clut == NULL)
        clutPoints = 0;
    else
        clutPoints    = clut->Params->nSamples[0];

    if (!_cmsWriteUInt8Number(io, (cmsUInt8Number) NewLUT ->InputChannels)) return FALSE;
    if (!_cmsWriteUInt8Number(io, (cmsUInt8Number) NewLUT ->OutputChannels)) return FALSE;
    if (!_cmsWriteUInt8Number(io, (cmsUInt8Number) clutPoints)) return FALSE;
    if (!_cmsWriteUInt8Number(io, 0)) return FALSE; // Padding


    if (MatMPE != NULL) {

        if (!_cmsWrite15Fixed16Number(io, MatMPE -> Double[0])) return FALSE;
        if (!_cmsWrite15Fixed16Number(io, MatMPE -> Double[1])) return FALSE;
        if (!_cmsWrite15Fixed16Number(io, MatMPE -> Double[2])) return FALSE;
        if (!_cmsWrite15Fixed16Number(io, MatMPE -> Double[3])) return FALSE;
        if (!_cmsWrite15Fixed16Number(io, MatMPE -> Double[4])) return FALSE;
        if (!_cmsWrite15Fixed16Number(io, MatMPE -> Double[5])) return FALSE;
        if (!_cmsWrite15Fixed16Number(io, MatMPE -> Double[6])) return FALSE;
        if (!_cmsWrite15Fixed16Number(io, MatMPE -> Double[7])) return FALSE;
        if (!_cmsWrite15Fixed16Number(io, MatMPE -> Double[8])) return FALSE;

    }
    else {

        if (!_cmsWrite15Fixed16Number(io, 1)) return FALSE;
        if (!_cmsWrite15Fixed16Number(io, 0)) return FALSE;
        if (!_cmsWrite15Fixed16Number(io, 0)) return FALSE;
        if (!_cmsWrite15Fixed16Number(io, 0)) return FALSE;
        if (!_cmsWrite15Fixed16Number(io, 1)) return FALSE;
        if (!_cmsWrite15Fixed16Number(io, 0)) return FALSE;
        if (!_cmsWrite15Fixed16Number(io, 0)) return FALSE;
        if (!_cmsWrite15Fixed16Number(io, 0)) return FALSE;
        if (!_cmsWrite15Fixed16Number(io, 1)) return FALSE;
    }

    // The prelinearization table
    if (!Write8bitTables(self ->ContextID, io, NewLUT ->InputChannels, PreMPE)) return FALSE;

    nTabSize = uipow(NewLUT->OutputChannels, clutPoints, NewLUT ->InputChannels);
    if (nTabSize == (cmsUInt32Number) -1) return FALSE;
    if (nTabSize > 0) {

        // The 3D CLUT.
        if (clut != NULL) {

            for (j=0; j < nTabSize; j++) {

                val = (cmsUInt8Number) FROM_16_TO_8(clut ->Tab.T[j]);
                if (!_cmsWriteUInt8Number(io, val)) return FALSE;
            }
        }
    }

    // The postlinearization table
    if (!Write8bitTables(self ->ContextID, io, NewLUT ->OutputChannels, PostMPE)) return FALSE;

    return TRUE;

    cmsUNUSED_PARAMETER(nItems);
}