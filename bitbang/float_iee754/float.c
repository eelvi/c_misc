

static inline void _strcpy(char *dest, const char *src)
{
    while (*src){
        *(dest++) = *(src++);
    }
}


static inline float d2f(char d)
{
    int n = (d - '0');
    return ((n > 10) || (n < 0)) ? 0 : n;
}

float _atof(const char *src)
{
    int is_negative = 0, fraction_idx = 0;
    float f = 0.0;
    while ((*src == ' ') || (*src == '\n') || (*src == '\t') || (*src == '-')){
        if (*src == '-'){
            is_negative = 1;
        }
        src++;
    }
    while (*src){
        if (((*src) >= '0') && ((*src) <= '9')){
            if (fraction_idx > 0){
                f += ( d2f(*src) / ((float) fraction_idx) );
                fraction_idx *= 10;
            }
            else {
                f *= 10.0;
                f += d2f(*src);
            }
        }
        else if (*src == '.'){
            //invalid: 2 points
            if (fraction_idx)
                break;
            fraction_idx = 10;
        }
        else{
            break;
        }
        src++;
    }
    return (is_negative)? -f :f;
}


//credit: opencv implementation
union iee754s{
    unsigned long u;
    float f;
};


static inline int _isnan( float f )
{
    union iee754s e;
    e.f = f;
    return  (e.u & 0x7fffffffu) > 0x7f800000u;
}
static inline int _isinf( float f )
{
    union iee754s e;
    e.f = f;
    return ((e.u & 0x7fffffffu) == 0x7f800000u)? ((e.u & 0x80000000u)? -1 : 1) : 0;
}
///////////////

void _ftoa(char *dest, float f, int p, int size)
{

    int base10_idx = 1, written_point = 0;
    int int_part = 0, frac_part = 0, write_len = 0, is_negative = 0;

    if ( _isnan(f) || _isinf(f) ){
        if (size > 5){
            if (_isnan(f))
                _strcpy(dest, "NaN");
            else if ( _isinf(f) == 1)
                _strcpy(dest, "+inf");
            else
                _strcpy(dest, "-inf");
        }
        else
            *dest = '\0';
        return;
    }

    if (f < 0.0){
        int_part++;
        is_negative = 1;
        f = -f;
    }
    else if (f == 0.0){
        p = 0;
    }

    if (f >= 10.0) //normalize MSD
        while (f >= 10.0){
            base10_idx++;
            f /= 10.0;
        }

    int_part += base10_idx;

    if ((int_part+1) > size){
        *(dest) = '\0';
        return;
    }
    else if (p > 0){
        frac_part = size - int_part - 2;
        frac_part = (frac_part > p)? (p) : ((frac_part > 0)? frac_part : 0);
    }
    write_len = int_part + frac_part;

    if (is_negative){
        write_len--;
        *(dest++) = '-';
    }
    for (int i=0; i<write_len; i++){
        if ((base10_idx-- <= 0) && (!written_point)){
            *(dest++) = '.';
            written_point = 1;
        }
        *(dest++) = (((int) f % 10) + '0');
        f *= 10.0;
    }
    
    *dest = '\0';
}
