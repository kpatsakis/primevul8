void VariationalRefinementImpl::calc(InputArray I0, InputArray I1, InputOutputArray flow)
{
    CV_Assert(!I0.empty() && I0.channels() == 1);
    CV_Assert(!I1.empty() && I1.channels() == 1);
    CV_Assert(I0.sameSize(I1));
    CV_Assert((I0.depth() == CV_8U && I1.depth() == CV_8U) || (I0.depth() == CV_32F && I1.depth() == CV_32F));
    CV_Assert(!flow.empty() && flow.depth() == CV_32F && flow.channels() == 2);
    CV_Assert(I0.sameSize(flow));

    Mat uv[2];
    Mat &flowMat = flow.getMatRef();
    split(flowMat, uv);
    calcUV(I0, I1, uv[0], uv[1]);
    merge(uv, 2, flowMat);
}