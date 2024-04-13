void *Type_MPE_Read(struct _cms_typehandler_struct* self, cmsIOHANDLER* io, cmsUInt32Number* nItems, cmsUInt32Number SizeOfTag)
{
    cmsUInt16Number InputChans, OutputChans;
    cmsUInt32Number ElementCount;
    cmsPipeline *NewLUT = NULL;
    cmsUInt32Number BaseOffset;

    // Get actual position as a basis for element offsets
    BaseOffset = io ->Tell(io) - sizeof(_cmsTagBase);

    // Read channels and element count
    if (!_cmsReadUInt16Number(io, &InputChans)) return NULL;
    if (!_cmsReadUInt16Number(io, &OutputChans)) return NULL;

    if (InputChans == 0 || InputChans >= cmsMAXCHANNELS) return NULL;
    if (OutputChans == 0 || OutputChans >= cmsMAXCHANNELS) return NULL;

    // Allocates an empty LUT
    NewLUT = cmsPipelineAlloc(self ->ContextID, InputChans, OutputChans);
    if (NewLUT == NULL) return NULL;

    if (!_cmsReadUInt32Number(io, &ElementCount)) goto Error;    
    if (!ReadPositionTable(self, io, ElementCount, BaseOffset, NewLUT, ReadMPEElem)) goto Error;

    // Check channel count
    if (InputChans != NewLUT->InputChannels ||
        OutputChans != NewLUT->OutputChannels) goto Error;

    // Success
    *nItems = 1;
    return NewLUT;

    // Error
Error:    
    if (NewLUT != NULL) cmsPipelineFree(NewLUT);
    *nItems = 0;
    return NULL;

    cmsUNUSED_PARAMETER(SizeOfTag);
}