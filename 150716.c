    void subtractOp(void *src1, void *src2, void *dst)
    {
        subtract(*(Mat *)src1, *(Mat *)src2, *(Mat *)dst, noArray(), CV_32F);
    }