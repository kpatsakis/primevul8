void* Type_LUTA2B_Read(struct _cms_typehandler_struct* self, cmsIOHANDLER* io, cmsUInt32Number* nItems, cmsUInt32Number SizeOfTag)
{
    cmsUInt32Number      BaseOffset;
    cmsUInt8Number       inputChan;      // Number of input channels
    cmsUInt8Number       outputChan;     // Number of output channels
    cmsUInt32Number      offsetB;        // Offset to first "B" curve
    cmsUInt32Number      offsetMat;      // Offset to matrix
    cmsUInt32Number      offsetM;        // Offset to first "M" curve
    cmsUInt32Number      offsetC;        // Offset to CLUT
    cmsUInt32Number      offsetA;        // Offset to first "A" curve
    cmsPipeline* NewLUT = NULL;


    BaseOffset = io ->Tell(io) - sizeof(_cmsTagBase);

    if (!_cmsReadUInt8Number(io, &inputChan)) return NULL;
    if (!_cmsReadUInt8Number(io, &outputChan)) return NULL;

    if (!_cmsReadUInt16Number(io, NULL)) return NULL;

    if (!_cmsReadUInt32Number(io, &offsetB)) return NULL;
    if (!_cmsReadUInt32Number(io, &offsetMat)) return NULL;
    if (!_cmsReadUInt32Number(io, &offsetM)) return NULL;
    if (!_cmsReadUInt32Number(io, &offsetC)) return NULL;
    if (!_cmsReadUInt32Number(io, &offsetA)) return NULL;

    if (inputChan == 0 || inputChan >= cmsMAXCHANNELS) return NULL;
    if (outputChan == 0 || outputChan >= cmsMAXCHANNELS) return NULL;

    // Allocates an empty LUT
    NewLUT = cmsPipelineAlloc(self ->ContextID, inputChan, outputChan);
    if (NewLUT == NULL) return NULL;

    if (offsetA!= 0) {
        if (!cmsPipelineInsertStage(NewLUT, cmsAT_END, ReadSetOfCurves(self, io, BaseOffset + offsetA, inputChan)))
            goto Error;
    }

    if (offsetC != 0) {
        if (!cmsPipelineInsertStage(NewLUT, cmsAT_END, ReadCLUT(self, io, BaseOffset + offsetC, inputChan, outputChan)))
            goto Error;
    }

    if (offsetM != 0) {
        if (!cmsPipelineInsertStage(NewLUT, cmsAT_END, ReadSetOfCurves(self, io, BaseOffset + offsetM, outputChan)))
            goto Error;
    }

    if (offsetMat != 0) {
        if (!cmsPipelineInsertStage(NewLUT, cmsAT_END, ReadMatrix(self, io, BaseOffset + offsetMat)))
            goto Error;
    }

    if (offsetB != 0) {
        if (!cmsPipelineInsertStage(NewLUT, cmsAT_END, ReadSetOfCurves(self, io, BaseOffset + offsetB, outputChan)))
            goto Error;
    }

    *nItems = 1;
    return NewLUT;
Error:
    cmsPipelineFree(NewLUT);
    return NULL;

    cmsUNUSED_PARAMETER(SizeOfTag);
}