cmsStage* ReadMatrix(struct _cms_typehandler_struct* self, cmsIOHANDLER* io, cmsUInt32Number Offset)
{
    cmsFloat64Number dMat[3*3];
    cmsFloat64Number dOff[3];
    cmsStage* Mat;

    // Go to address
    if (!io -> Seek(io, Offset)) return NULL;

    // Read the Matrix
    if (!_cmsRead15Fixed16Number(io, &dMat[0])) return NULL;
    if (!_cmsRead15Fixed16Number(io, &dMat[1])) return NULL;
    if (!_cmsRead15Fixed16Number(io, &dMat[2])) return NULL;
    if (!_cmsRead15Fixed16Number(io, &dMat[3])) return NULL;
    if (!_cmsRead15Fixed16Number(io, &dMat[4])) return NULL;
    if (!_cmsRead15Fixed16Number(io, &dMat[5])) return NULL;
    if (!_cmsRead15Fixed16Number(io, &dMat[6])) return NULL;
    if (!_cmsRead15Fixed16Number(io, &dMat[7])) return NULL;
    if (!_cmsRead15Fixed16Number(io, &dMat[8])) return NULL;

    if (!_cmsRead15Fixed16Number(io, &dOff[0])) return NULL;
    if (!_cmsRead15Fixed16Number(io, &dOff[1])) return NULL;
    if (!_cmsRead15Fixed16Number(io, &dOff[2])) return NULL;

    Mat = cmsStageAllocMatrix(self ->ContextID, 3, 3, dMat, dOff);

     return Mat;
}