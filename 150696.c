bool DISOpticalFlowImpl::ocl_PatchInverseSearch(UMat &src_Ux, UMat &src_Uy,
                                                UMat &I0, UMat &I1, UMat &I0x, UMat &I0y, int num_iter, int pyr_level)
{
    size_t globalSize[] = {(size_t)ws, (size_t)hs};
    size_t localSize[]  = {16, 16};
    int idx;
    int num_inner_iter = (int)floor(grad_descent_iter / (float)num_iter);

    for (int iter = 0; iter < num_iter; iter++)
    {
        if (iter == 0)
        {
            ocl::Kernel k1("dis_patch_inverse_search_fwd_1", ocl::video::dis_flow_oclsrc);
            size_t global_sz[] = {(size_t)hs * 8};
            size_t local_sz[]  = {8};
            idx = 0;

            idx = k1.set(idx, ocl::KernelArg::PtrReadOnly(src_Ux));
            idx = k1.set(idx, ocl::KernelArg::PtrReadOnly(src_Uy));
            idx = k1.set(idx, ocl::KernelArg::PtrReadOnly(I0));
            idx = k1.set(idx, ocl::KernelArg::PtrReadOnly(I1));
            idx = k1.set(idx, (int)border_size);
            idx = k1.set(idx, (int)patch_size);
            idx = k1.set(idx, (int)patch_stride);
            idx = k1.set(idx, (int)w);
            idx = k1.set(idx, (int)h);
            idx = k1.set(idx, (int)ws);
            idx = k1.set(idx, (int)hs);
            idx = k1.set(idx, (int)pyr_level);
            idx = k1.set(idx, ocl::KernelArg::PtrWriteOnly(u_Sx));
            idx = k1.set(idx, ocl::KernelArg::PtrWriteOnly(u_Sy));
            if (!k1.run(1, global_sz, local_sz, false))
                return false;

            ocl::Kernel k2("dis_patch_inverse_search_fwd_2", ocl::video::dis_flow_oclsrc);
            idx = 0;

            idx = k2.set(idx, ocl::KernelArg::PtrReadOnly(src_Ux));
            idx = k2.set(idx, ocl::KernelArg::PtrReadOnly(src_Uy));
            idx = k2.set(idx, ocl::KernelArg::PtrReadOnly(I0));
            idx = k2.set(idx, ocl::KernelArg::PtrReadOnly(I1));
            idx = k2.set(idx, ocl::KernelArg::PtrReadOnly(I0x));
            idx = k2.set(idx, ocl::KernelArg::PtrReadOnly(I0y));
            idx = k2.set(idx, ocl::KernelArg::PtrReadOnly(u_I0xx_buf));
            idx = k2.set(idx, ocl::KernelArg::PtrReadOnly(u_I0yy_buf));
            idx = k2.set(idx, ocl::KernelArg::PtrReadOnly(u_I0xy_buf));
            idx = k2.set(idx, ocl::KernelArg::PtrReadOnly(u_I0x_buf));
            idx = k2.set(idx, ocl::KernelArg::PtrReadOnly(u_I0y_buf));
            idx = k2.set(idx, (int)border_size);
            idx = k2.set(idx, (int)patch_size);
            idx = k2.set(idx, (int)patch_stride);
            idx = k2.set(idx, (int)w);
            idx = k2.set(idx, (int)h);
            idx = k2.set(idx, (int)ws);
            idx = k2.set(idx, (int)hs);
            idx = k2.set(idx, (int)num_inner_iter);
            idx = k2.set(idx, (int)pyr_level);
            idx = k2.set(idx, ocl::KernelArg::PtrReadWrite(u_Sx));
            idx = k2.set(idx, ocl::KernelArg::PtrReadWrite(u_Sy));
            if (!k2.run(2, globalSize, localSize, false))
                return false;
        }
        else
        {
            ocl::Kernel k3("dis_patch_inverse_search_bwd_1", ocl::video::dis_flow_oclsrc);
            size_t global_sz[] = {(size_t)hs * 8};
            size_t local_sz[]  = {8};
            idx = 0;

            idx = k3.set(idx, ocl::KernelArg::PtrReadOnly(I0));
            idx = k3.set(idx, ocl::KernelArg::PtrReadOnly(I1));
            idx = k3.set(idx, (int)border_size);
            idx = k3.set(idx, (int)patch_size);
            idx = k3.set(idx, (int)patch_stride);
            idx = k3.set(idx, (int)w);
            idx = k3.set(idx, (int)h);
            idx = k3.set(idx, (int)ws);
            idx = k3.set(idx, (int)hs);
            idx = k3.set(idx, (int)pyr_level);
            idx = k3.set(idx, ocl::KernelArg::PtrReadWrite(u_Sx));
            idx = k3.set(idx, ocl::KernelArg::PtrReadWrite(u_Sy));
            if (!k3.run(1, global_sz, local_sz, false))
                return false;

            ocl::Kernel k4("dis_patch_inverse_search_bwd_2", ocl::video::dis_flow_oclsrc);
            idx = 0;

            idx = k4.set(idx, ocl::KernelArg::PtrReadOnly(I0));
            idx = k4.set(idx, ocl::KernelArg::PtrReadOnly(I1));
            idx = k4.set(idx, ocl::KernelArg::PtrReadOnly(I0x));
            idx = k4.set(idx, ocl::KernelArg::PtrReadOnly(I0y));
            idx = k4.set(idx, ocl::KernelArg::PtrReadOnly(u_I0xx_buf));
            idx = k4.set(idx, ocl::KernelArg::PtrReadOnly(u_I0yy_buf));
            idx = k4.set(idx, ocl::KernelArg::PtrReadOnly(u_I0xy_buf));
            idx = k4.set(idx, ocl::KernelArg::PtrReadOnly(u_I0x_buf));
            idx = k4.set(idx, ocl::KernelArg::PtrReadOnly(u_I0y_buf));
            idx = k4.set(idx, (int)border_size);
            idx = k4.set(idx, (int)patch_size);
            idx = k4.set(idx, (int)patch_stride);
            idx = k4.set(idx, (int)w);
            idx = k4.set(idx, (int)h);
            idx = k4.set(idx, (int)ws);
            idx = k4.set(idx, (int)hs);
            idx = k4.set(idx, (int)num_inner_iter);
            idx = k4.set(idx, ocl::KernelArg::PtrReadWrite(u_Sx));
            idx = k4.set(idx, ocl::KernelArg::PtrReadWrite(u_Sy));
            if (!k4.run(2, globalSize, localSize, false))
                return false;
        }
    }
    return true;
}