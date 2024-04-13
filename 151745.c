cmsBool  WriteMatrix(struct _cms_typehandler_struct* self, cmsIOHANDLER* io, cmsStage* mpe)
{
    _cmsStageMatrixData* m = (_cmsStageMatrixData*) mpe -> Data;

    // Write the Matrix
    if (!_cmsWrite15Fixed16Number(io, m -> Double[0])) return FALSE;
    if (!_cmsWrite15Fixed16Number(io, m -> Double[1])) return FALSE;
    if (!_cmsWrite15Fixed16Number(io, m -> Double[2])) return FALSE;
    if (!_cmsWrite15Fixed16Number(io, m -> Double[3])) return FALSE;
    if (!_cmsWrite15Fixed16Number(io, m -> Double[4])) return FALSE;
    if (!_cmsWrite15Fixed16Number(io, m -> Double[5])) return FALSE;
    if (!_cmsWrite15Fixed16Number(io, m -> Double[6])) return FALSE;
    if (!_cmsWrite15Fixed16Number(io, m -> Double[7])) return FALSE;
    if (!_cmsWrite15Fixed16Number(io, m -> Double[8])) return FALSE;

    if (m ->Offset != NULL) {

    if (!_cmsWrite15Fixed16Number(io, m -> Offset[0])) return FALSE;
    if (!_cmsWrite15Fixed16Number(io, m -> Offset[1])) return FALSE;
    if (!_cmsWrite15Fixed16Number(io, m -> Offset[2])) return FALSE;
    }
    else {
        if (!_cmsWrite15Fixed16Number(io, 0)) return FALSE;
        if (!_cmsWrite15Fixed16Number(io, 0)) return FALSE;
        if (!_cmsWrite15Fixed16Number(io, 0)) return FALSE;

    }


    return TRUE;

    cmsUNUSED_PARAMETER(self);
}