#include "s21_decimal.h"

int s21_is_equal(s21_decimal one, s21_decimal two) {
    scale_dec(&one, &two);
    s21_decimal res = {{0, 0, 0, 0}};
    int result = 1;
    for (int i = 0; i < 4 && result == 1; i++) {
        res.bits[i] = one.bits[i] ^ two.bits[i];
        if (res.bits[i] != 0) result = 0;
    }
    return result;
}

int s21_is_not_equal(s21_decimal one, s21_decimal two) {
    return (s21_is_equal(one, two) == 1) ? 0 : 1;
}

int s21_is_less(s21_decimal one, s21_decimal two) {
    scale_dec(&one, &two);
    int sym1 = check_bit(one, 127);
    int sym2 = check_bit(two, 127);
    int result = 0;
    if (sym1 == 0 && sym2 == 0) {
        if (less_or_great(one, two) == 0) result = 1;
    } else if (sym1 == 1 && sym2 == 1) {
        if (less_or_great(one, two) == 1) result = 1;
    } else if (sym1 == 1 && sym2 == 0) {
        result = 1;
    } else if (sym1 == 0 && sym2 == 1) {
        result = 0;
    }
    return result;
}

int s21_is_less_or_equal(s21_decimal one, s21_decimal two) {
    int result = 0;
    if (s21_is_less(one, two) == 1 || s21_is_equal(one, two) == 1) result = 1;
    return result;
}

int s21_is_greater(s21_decimal one, s21_decimal two) {
    scale_dec(&one, &two);
    int sym1 = check_bit(one, 127);
    int sym2 = check_bit(two, 127);
    int result = 0;
    if (sym1 == 0 && sym2 == 0) {
        if (less_or_great(one, two) == 1) result = 1;
    } else if (sym1 == 1 && sym2 == 1) {
        if (less_or_great(one, two) == 0) result = 1;
    } else if (sym1 == 1 && sym2 == 0) {
        result = 0;
    } else if (sym1 == 0 && sym2 == 1) {
        result = 1;
    }
    return result;
}

int s21_is_greater_or_equal(s21_decimal one, s21_decimal two) {
    int result = 0;
    if (s21_is_greater(one, two) == 1 || s21_is_equal(one, two) == 1) result = 1;
    return result;
}

int less_or_great(s21_decimal one, s21_decimal two) {
    int bits = 95;
    int res = -1;
    for (int i = 2; i >= 0 && res == -1; i--) {
        for (int j = 31; j >= 0 && res == -1; j--) {
            int bit_one = check_bit(one, bits);
            int bit_two = check_bit(two, bits);
            if (bit_one == 1 && bit_two == 0) res = 1;
            if (bit_one == 0 && bit_two == 1) res = 0;
            bits--;
        }
    }
    return res;
}
