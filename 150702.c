DISOpticalFlowImpl::DISOpticalFlowImpl()
{
    finest_scale = 2;
    patch_size = 8;
    patch_stride = 4;
    grad_descent_iter = 16;
    variational_refinement_iter = 5;
    variational_refinement_alpha = 20.f;
    variational_refinement_gamma = 10.f;
    variational_refinement_delta = 5.f;

    border_size = 16;
    use_mean_normalization = true;
    use_spatial_propagation = true;

    /* Use separate variational refinement instances for different scales to avoid repeated memory allocation: */
    int max_possible_scales = 10;
    for (int i = 0; i < max_possible_scales; i++)
        variational_refinement_processors.push_back(VariationalRefinement::create());
}