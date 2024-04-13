void VariationalRefinementImpl::prepareBuffers(Mat &I0, Mat &I1, Mat &W_u, Mat &W_v)
{
    Size s = I0.size();
    A11.create(s);
    A12.create(s);
    A22.create(s);
    b1.create(s);
    b2.create(s);
    weights.create(s);
    weights.red.setTo(0.0f);
    weights.black.setTo(0.0f);
    tempW_u.create(s);
    tempW_v.create(s);
    dW_u.create(s);
    dW_v.create(s);
    W_u_rb.create(s);
    W_v_rb.create(s);

    Ix.create(s);
    Iy.create(s);
    Iz.create(s);
    Ixx.create(s);
    Ixy.create(s);
    Iyy.create(s);
    Ixz.create(s);
    Iyz.create(s);

    Ix_rb.create(s);
    Iy_rb.create(s);
    Iz_rb.create(s);
    Ixx_rb.create(s);
    Ixy_rb.create(s);
    Iyy_rb.create(s);
    Ixz_rb.create(s);
    Iyz_rb.create(s);

    mapX.create(s);
    mapY.create(s);

    /* Floating point warps work significantly better than fixed-point */
    Mat I1flt, warpedI;
    I1.convertTo(I1flt, CV_32F);
    warpImage(warpedI, I1flt, W_u, W_v);

    /* Computing an average of the current and warped next frames (to compute the derivatives on) and
     * temporal derivative Iz
     */
    Mat averagedI;
    {
        vector<void *> op1s;
        op1s.push_back((void *)&I0);
        op1s.push_back((void *)&warpedI);
        vector<void *> op2s;
        op2s.push_back((void *)&warpedI);
        op2s.push_back((void *)&I0);
        vector<void *> op3s;
        op3s.push_back((void *)&averagedI);
        op3s.push_back((void *)&Iz);
        vector<Op> ops;
        ops.push_back(&VariationalRefinementImpl::averageOp);
        ops.push_back(&VariationalRefinementImpl::subtractOp);
        parallel_for_(Range(0, 2), ParallelOp_ParBody(*this, ops, op1s, op2s, op3s));
    }
    splitCheckerboard(Iz_rb, Iz);

    /* Computing first-order derivatives */
    {
        vector<void *> op1s;
        op1s.push_back((void *)&averagedI);
        op1s.push_back((void *)&averagedI);
        op1s.push_back((void *)&Iz);
        op1s.push_back((void *)&Iz);
        vector<void *> op2s;
        op2s.push_back((void *)&Ix);
        op2s.push_back((void *)&Iy);
        op2s.push_back((void *)&Ixz);
        op2s.push_back((void *)&Iyz);
        vector<void *> op3s;
        op3s.push_back((void *)&Ix_rb);
        op3s.push_back((void *)&Iy_rb);
        op3s.push_back((void *)&Ixz_rb);
        op3s.push_back((void *)&Iyz_rb);
        vector<Op> ops;
        ops.push_back(&VariationalRefinementImpl::gradHorizAndSplitOp);
        ops.push_back(&VariationalRefinementImpl::gradVertAndSplitOp);
        ops.push_back(&VariationalRefinementImpl::gradHorizAndSplitOp);
        ops.push_back(&VariationalRefinementImpl::gradVertAndSplitOp);
        parallel_for_(Range(0, 4), ParallelOp_ParBody(*this, ops, op1s, op2s, op3s));
    }

    /* Computing second-order derivatives */
    {
        vector<void *> op1s;
        op1s.push_back((void *)&Ix);
        op1s.push_back((void *)&Ix);
        op1s.push_back((void *)&Iy);
        vector<void *> op2s;
        op2s.push_back((void *)&Ixx);
        op2s.push_back((void *)&Ixy);
        op2s.push_back((void *)&Iyy);
        vector<void *> op3s;
        op3s.push_back((void *)&Ixx_rb);
        op3s.push_back((void *)&Ixy_rb);
        op3s.push_back((void *)&Iyy_rb);
        vector<Op> ops;
        ops.push_back(&VariationalRefinementImpl::gradHorizAndSplitOp);
        ops.push_back(&VariationalRefinementImpl::gradVertAndSplitOp);
        ops.push_back(&VariationalRefinementImpl::gradVertAndSplitOp);
        parallel_for_(Range(0, 3), ParallelOp_ParBody(*this, ops, op1s, op2s, op3s));
    }
}