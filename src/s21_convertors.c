#include "s21_decimal.h"

int s21_from_decimal_to_float(s21_decimal src, float *dst) {
    int res = 0;
    if (dst == NULL) {
        res = 1;
    } else {
        double tmp = 0.0;
        *dst = 0.0;
        for (int i = 0; i < 96; i++) {
            if (check_bit(src, i) == 1) tmp += pow(2, i);
        }
        int scale = check_scale(src);
        while (scale > 0) {
            tmp /= 10.0;
            scale--;
        }
    if (check_bit(src, 127) == 1) tmp *= -1;
    *dst = (float)tmp;
    }
    return res;
}

int s21_from_decimal_to_int(s21_decimal src, int *dst) {
    int res = 0;
    s21_decimal x = {{0, 0, 0, 0}};
    s21_truncate(src, &x);
    if (dst == NULL) {
        res = 1;
    } else {
        *dst = 0;
        if (zero_left(x) <= 63) {
            res = 1;
        } else {
            *dst = x.bits[0];
            if (check_bit(x, 127) == 1) *dst *= -1;
        }
    }
    return res;
}

int s21_from_float_to_decimal(float src, s21_decimal *dst) {
    int res = 0;
    if ((0 < src && src < minimum) || src == s21_NAN || src == s21_INF || src == -s21_INF || dst == NULL) {
        res = 1;
    } else {
        int scale = 0;
    for (int i = 0; i < 4; ++i) dst->bits[i] = 0;
    if (src < 0) {
        set_bit(dst, 127, 1);
        src *= -1;
    }
    double new_src = (double)fabs(src);
        int float_exp = get_float_exp(&src);
        while (scale < 28 && (int) new_src / (int) pow(2, 21) == 0) {
            new_src *= 10;
            scale++;
        }
        new_src = round(new_src);
        if (scale <= 28 || (float_exp > -94 && float_exp < 96)) {
            src = (float)new_src;
            while (fmod(src, 10) == 0 && scale > 0) {
                src /= 10;
                scale--;
            }
            float_exp = get_float_exp(&src);
            dst->bits[float_exp / 32] |= 1 << float_exp % 32;
            int int_part = get_float_bit(src);
            for (int i = float_exp - 1, j = 22; j >= 0; i--, j--) {
                if ((int_part & (1 << j)) != 0) {
                    dst->bits[i / 32] |= 1 << i % 32;
                }
            }
        }
            change_scale(dst, scale);
    }
    return res;
}

int s21_from_int_to_decimal(int src, s21_decimal *dst) {
    int res = 0;
    if (dst == NULL || src == s21_INF || src == s21_NAN) res = 1;
    if (dst && res == 0) {
    for (int i = 0; i < 4; ++i) dst->bits[i] = 0;
    if (src < 0) {
        set_bit(dst, 127, 1);
        src *= -1;
        }
    dst->bits[0] = src;
    }
    return res;
}
