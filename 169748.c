static inline UINT8 clip8(float in)
{
    int out = (int) in;
    if (out >= 255)
       return 255;
    if (out <= 0)
        return 0;
    return (UINT8) out;
}