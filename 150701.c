void VariationalRefinementImpl::warpImage(Mat &dst, Mat &src, Mat &flow_u, Mat &flow_v)
{
    for (int i = 0; i < flow_u.rows; i++)
    {
        float *pFlowU = flow_u.ptr<float>(i);
        float *pFlowV = flow_v.ptr<float>(i);
        float *pMapX = mapX.ptr<float>(i);
        float *pMapY = mapY.ptr<float>(i);
        for (int j = 0; j < flow_u.cols; j++)
        {
            pMapX[j] = j + pFlowU[j];
            pMapY[j] = i + pFlowV[j];
        }
    }
    remap(src, dst, mapX, mapY, INTER_LINEAR, BORDER_REPLICATE);
}