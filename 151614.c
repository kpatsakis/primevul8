cmsBool Type_LUTA2B_Write(struct _cms_typehandler_struct* self, cmsIOHANDLER* io, void* Ptr, cmsUInt32Number nItems)
{
    cmsPipeline* Lut = (cmsPipeline*) Ptr;
    int inputChan, outputChan;
    cmsStage *A = NULL, *B = NULL, *M = NULL;
    cmsStage * Matrix = NULL;
    cmsStage * CLUT = NULL;
    cmsUInt32Number offsetB = 0, offsetMat = 0, offsetM = 0, offsetC = 0, offsetA = 0;
    cmsUInt32Number BaseOffset, DirectoryPos, CurrentPos;

    // Get the base for all offsets
    BaseOffset = io ->Tell(io) - sizeof(_cmsTagBase);

    if (Lut ->Elements != NULL)
        if (!cmsPipelineCheckAndRetreiveStages(Lut, 1, cmsSigCurveSetElemType, &B))
            if (!cmsPipelineCheckAndRetreiveStages(Lut, 3, cmsSigCurveSetElemType, cmsSigMatrixElemType, cmsSigCurveSetElemType, &M, &Matrix, &B))
                if (!cmsPipelineCheckAndRetreiveStages(Lut, 3, cmsSigCurveSetElemType, cmsSigCLutElemType, cmsSigCurveSetElemType, &A, &CLUT, &B))
                    if (!cmsPipelineCheckAndRetreiveStages(Lut, 5, cmsSigCurveSetElemType, cmsSigCLutElemType, cmsSigCurveSetElemType,
                        cmsSigMatrixElemType, cmsSigCurveSetElemType, &A, &CLUT, &M, &Matrix, &B)) {

                            cmsSignalError(self->ContextID, cmsERROR_NOT_SUITABLE, "LUT is not suitable to be saved as LutAToB");
                            return FALSE;
                    }

    // Get input, output channels
    inputChan  = cmsPipelineInputChannels(Lut);
    outputChan = cmsPipelineOutputChannels(Lut);

    // Write channel count
    if (!_cmsWriteUInt8Number(io, (cmsUInt8Number) inputChan)) return FALSE;
    if (!_cmsWriteUInt8Number(io, (cmsUInt8Number) outputChan)) return FALSE;
    if (!_cmsWriteUInt16Number(io, 0)) return FALSE;

    // Keep directory to be filled latter
    DirectoryPos = io ->Tell(io);

    // Write the directory
    if (!_cmsWriteUInt32Number(io, 0)) return FALSE;
    if (!_cmsWriteUInt32Number(io, 0)) return FALSE;
    if (!_cmsWriteUInt32Number(io, 0)) return FALSE;
    if (!_cmsWriteUInt32Number(io, 0)) return FALSE;
    if (!_cmsWriteUInt32Number(io, 0)) return FALSE;

    if (A != NULL) {

        offsetA = io ->Tell(io) - BaseOffset;
        if (!WriteSetOfCurves(self, io, cmsSigParametricCurveType, A)) return FALSE;
    }

    if (CLUT != NULL) {
        offsetC = io ->Tell(io) - BaseOffset;
        if (!WriteCLUT(self, io, Lut ->SaveAs8Bits ? 1 : 2, CLUT)) return FALSE;

    }
    if (M != NULL) {

        offsetM = io ->Tell(io) - BaseOffset;
        if (!WriteSetOfCurves(self, io, cmsSigParametricCurveType, M)) return FALSE;
    }

    if (Matrix != NULL) {
        offsetMat = io ->Tell(io) - BaseOffset;
        if (!WriteMatrix(self, io, Matrix)) return FALSE;
    }

    if (B != NULL) {

        offsetB = io ->Tell(io) - BaseOffset;
        if (!WriteSetOfCurves(self, io, cmsSigParametricCurveType, B)) return FALSE;
    }

    CurrentPos = io ->Tell(io);

    if (!io ->Seek(io, DirectoryPos)) return FALSE;

    if (!_cmsWriteUInt32Number(io, offsetB)) return FALSE;
    if (!_cmsWriteUInt32Number(io, offsetMat)) return FALSE;
    if (!_cmsWriteUInt32Number(io, offsetM)) return FALSE;
    if (!_cmsWriteUInt32Number(io, offsetC)) return FALSE;
    if (!_cmsWriteUInt32Number(io, offsetA)) return FALSE;

    if (!io ->Seek(io, CurrentPos)) return FALSE;

    return TRUE;

    cmsUNUSED_PARAMETER(nItems);
}