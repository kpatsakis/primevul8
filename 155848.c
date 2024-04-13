opj_image_t* opj_image_create0(void)
{
    opj_image_t *image = (opj_image_t*)opj_calloc(1, sizeof(opj_image_t));
    return image;
}