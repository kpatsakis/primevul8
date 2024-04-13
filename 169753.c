static float __attribute__((always_inline)) i2f(int v) {
    float x;
    __asm__("xorps %0, %0; cvtsi2ss %1, %0" : "=X"(x) : "r"(v) );
    return x;
}