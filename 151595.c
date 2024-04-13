int XFormSampler16(register const cmsUInt16Number In[], register cmsUInt16Number Out[], register void* Cargo)
{
    cmsPipeline* Lut = (cmsPipeline*) Cargo;
    cmsFloat32Number InFloat[cmsMAXCHANNELS], OutFloat[cmsMAXCHANNELS];
    cmsUInt32Number i;

    _cmsAssert(Lut -> InputChannels < cmsMAXCHANNELS);
    _cmsAssert(Lut -> OutputChannels < cmsMAXCHANNELS);

    // From 16 bit to floating point
    for (i=0; i < Lut ->InputChannels; i++)
        InFloat[i] = (cmsFloat32Number) (In[i] / 65535.0);

    // Evaluate in floating point
    cmsPipelineEvalFloat(InFloat, OutFloat, Lut);

    // Back to 16 bits representation
    for (i=0; i < Lut ->OutputChannels; i++)
        Out[i] = _cmsQuickSaturateWord(OutFloat[i] * 65535.0);

    // Always succeed
    return TRUE;
}