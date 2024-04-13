void VariationalRefinementImpl::calcUV(InputArray I0, InputArray I1, InputOutputArray flow_u, InputOutputArray flow_v)
{
    CV_Assert(!I0.empty() && I0.channels() == 1);
    CV_Assert(!I1.empty() && I1.channels() == 1);
    CV_Assert(I0.sameSize(I1));
    CV_Assert((I0.depth() == CV_8U && I1.depth() == CV_8U) || (I0.depth() == CV_32F && I1.depth() == CV_32F));
    CV_Assert(!flow_u.empty() && flow_u.depth() == CV_32F && flow_u.channels() == 1);
    CV_Assert(!flow_v.empty() && flow_v.depth() == CV_32F && flow_v.channels() == 1);
    CV_Assert(I0.sameSize(flow_u));
    CV_Assert(flow_u.sameSize(flow_v));

    int num_stripes = getNumThreads();
    Mat I0Mat = I0.getMat();
    Mat I1Mat = I1.getMat();
    Mat &W_u = flow_u.getMatRef();
    Mat &W_v = flow_v.getMatRef();
    prepareBuffers(I0Mat, I1Mat, W_u, W_v);

    splitCheckerboard(W_u_rb, W_u);
    splitCheckerboard(W_v_rb, W_v);
    W_u_rb.red.copyTo(tempW_u.red);
    W_u_rb.black.copyTo(tempW_u.black);
    W_v_rb.red.copyTo(tempW_v.red);
    W_v_rb.black.copyTo(tempW_v.black);
    dW_u.red.setTo(0.0f);
    dW_u.black.setTo(0.0f);
    dW_v.red.setTo(0.0f);
    dW_v.black.setTo(0.0f);

    for (int i = 0; i < fixedPointIterations; i++)
    {
        parallel_for_(Range(0, num_stripes), ComputeDataTerm_ParBody(*this, num_stripes, I0Mat.rows, dW_u, dW_v, true));
        parallel_for_(Range(0, num_stripes), ComputeDataTerm_ParBody(*this, num_stripes, I0Mat.rows, dW_u, dW_v, false));

        parallel_for_(Range(0, num_stripes), ComputeSmoothnessTermHorPass_ParBody(
                                               *this, num_stripes, I0Mat.rows, W_u_rb, W_v_rb, tempW_u, tempW_v, true));
        parallel_for_(Range(0, num_stripes), ComputeSmoothnessTermHorPass_ParBody(
                                               *this, num_stripes, I0Mat.rows, W_u_rb, W_v_rb, tempW_u, tempW_v, false));

        parallel_for_(Range(0, num_stripes),
                      ComputeSmoothnessTermVertPass_ParBody(*this, num_stripes, I0Mat.rows, W_u_rb, W_v_rb, true));
        parallel_for_(Range(0, num_stripes),
                      ComputeSmoothnessTermVertPass_ParBody(*this, num_stripes, I0Mat.rows, W_u_rb, W_v_rb, false));

        for (int j = 0; j < sorIterations; j++)
        {
            parallel_for_(Range(0, num_stripes), RedBlackSOR_ParBody(*this, num_stripes, I0Mat.rows, dW_u, dW_v, true));
            parallel_for_(Range(0, num_stripes), RedBlackSOR_ParBody(*this, num_stripes, I0Mat.rows, dW_u, dW_v, false));
        }

        tempW_u.red = W_u_rb.red + dW_u.red;
        tempW_u.black = W_u_rb.black + dW_u.black;
        updateRepeatedBorders(tempW_u);
        tempW_v.red = W_v_rb.red + dW_v.red;
        tempW_v.black = W_v_rb.black + dW_v.black;
        updateRepeatedBorders(tempW_v);
    }
    mergeCheckerboard(W_u, tempW_u);
    mergeCheckerboard(W_v, tempW_v);
}