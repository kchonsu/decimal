#include "s21_decimal.h"

int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
    int check = 0;
    int scale_dif = (int)check_scale(value_1) - (int)check_scale(value_2);
    int ch_mt = 0;
    if (scale_dif < 0) {
        ch_mt = (-1) * scale_dif;
    }
    int sign = 0;
    plant_sign(value_1, value_2, &sign);
    if (!null_dec(value_2) && !null_dec(value_1)) {
        value_1.bits[3] = 0;
        value_2.bits[3] = 0;
        div_correct(&value_1, &value_2, &scale_dif);
        s21_decimal value1 = value_1;
        s21_decimal value2 = value_2;
        int check_rem = 0;
        int count_int = 0;
        while (!null_dec(value1) && scale_dif < (29 - ch_mt)) {
            s21_decimal result_buf = {0};
            int count_shift = 0;
            while (count_bit(value1) > count_bit(value2)) {
                shift_left(&value2, 1);
                count_shift++;
            }
            s21_decimal minus_value2 = value2;
            reverse(&minus_value2, 0);
            reverse(&minus_value2, 1);
            reverse(&minus_value2, 2);
            s21_decimal one = {{1, 0, 0, 0}};
            s21_decimal sum = {0};
            sumbits(minus_value2, one, &minus_value2);
            sumbits(value1, minus_value2, &value1);
            int left_bit = check_bit(value1, 95);
            left_bit ^= (1 << 0);
            set_bit(&result_buf, 0, left_bit);
            int check_div = s21_is_greater_or_equal(value1, value_2);
            int count_shift_for_rem = 0;
            while (count_shift > 0 && !null_dec(value1) && check_div) {
                shift_left(&value1, 1);
                if (left_bit == 1) {
                    sumbits(value1, minus_value2, &value1);
                }
                if (left_bit == 0) {
                    sumbits(value1, value2, &value1);
                }
                left_bit = check_bit(value1, 95);
                left_bit ^= (1 << 0);
                shift_left(&result_buf, 1);
                set_bit(&result_buf, 0, left_bit);
                check_div = s21_is_greater_or_equal(value1, value_2);
                count_shift--;
                count_shift_for_rem++;
            }
            while (count_shift > 0) {
                shift_left(&result_buf, 1);
                set_bit(&result_buf, 0, 0);
                count_shift--;
            }
            if (scale_dif < (28 - ch_mt)) {
                if (!check_rem) {
                    *result = result_buf;
                    s21_decimal res = *result;
                    while (!null_dec(res)) {
                        div_ten(&res);
                        count_int++;
                    }
                } else {
                    multen(result);
                    sumbits(*result, result_buf, &sum);
                    result->bits[0] = sum.bits[0];
                    result->bits[1] = sum.bits[1];
                    result->bits[2] = sum.bits[2];
                    scale_dif++;
                }
                if (check_bit(value1, 95) == 1) {
                    sumbits(value1, value2, &value1);
                }
                shift_right(&value1, count_shift_for_rem);
                multen(&value1);
                if (s21_is_less(value1, value_2) && check_rem && !null_dec(value1)) {
                    multen(result);
                }
                if (!null_dec(value1)) {
                    value2 = value_2;
                    div_correct(&value1, &value2, &scale_dif);
                }
                check_rem++;
            } else {
                if (result_buf.bits[0] >= 5) {
                    s21_decimal one = {{1, 0, 0, 0}};
                    sumbits(*result, one, result);
                }
                scale_dif++;
            }
        }
        if (scale_dif == (29 - ch_mt)) {
            scale_dif--;
        }
        if (count_int + ch_mt <= 29) {
            while (scale_dif < 0) {
            multen(result);
            scale_dif++;
            }
        } else {
            check = 1;
            result->bits[0] = 0;
            result->bits[1] = 0;
            result->bits[2] = 0;
            result->bits[3] = 0;
        }
        null_scale(result);
        change_scale(result, scale_dif);
        set_sign(result, sign);
        if (null_dec(*result)) {
            check = 2;
            result->bits[0] = 0;
            result->bits[1] = 0;
            result->bits[2] = 0;
            result->bits[3] = 0;
        }
    } else {
        check = 3;
    }
    return check;
}

int div_correct(s21_decimal *value_1, s21_decimal *value_2, int *scale_dif) {
    if (s21_is_less(*value_1, *value_2)) {
        while (s21_is_less(*value_1, *value_2) == 1) {
            multen(value_1);
            (*scale_dif)++;
        }
        while (s21_is_less(*value_1, *value_2) == 1) {
            div_ten(value_2);
            (*scale_dif)++;
        }
    }
  return 0;
}
