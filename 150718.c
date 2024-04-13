void VariationalRefinementImpl::collectGarbage()
{
    Ix.release();
    Iy.release();
    Iz.release();
    Ixx.release();
    Ixy.release();
    Iyy.release();
    Ixz.release();
    Iyz.release();

    Ix_rb.release();
    Iy_rb.release();
    Iz_rb.release();
    Ixx_rb.release();
    Ixy_rb.release();
    Iyy_rb.release();
    Ixz_rb.release();
    Iyz_rb.release();

    A11.release();
    A12.release();
    A22.release();
    b1.release();
    b2.release();
    weights.release();

    mapX.release();
    mapY.release();

    tempW_u.release();
    tempW_v.release();
    dW_u.release();
    dW_v.release();
    W_u_rb.release();
    W_v_rb.release();
}