union iee754sf{
    unsigned long u;
    float f;
};


int to_int(float f)
{
    union iee754sf e;
    unsigned long sign, uexp, coeff;
    long exp;
    int r;

    e.f = f;

    //masks
    sign  = (e.u & 0x80000000u) >> 31;
    uexp   = (e.u & 0x7f800000u) >> 23;
    exp = uexp - 127;
    coeff = (e.u & 0x007fffffu) | (0x1 << 23);

    if ((exp > 0) && (exp <= 23)){
        r = coeff >> (23 - exp);
        return (sign)? -r : r;
    }

    return 0;
}
