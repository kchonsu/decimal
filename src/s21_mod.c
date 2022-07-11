#include "s21_decimal.h"

int s21_mod(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
    int check = 0;
    scale_dec(&value_1, &value_2);
    if (s21_is_greater(value_2, value_1)) {
        result->bits[0] = value_1.bits[0];
        result->bits[1] = value_1.bits[1];
        result->bits[2] = value_1.bits[2];
        result->bits[3] = value_1.bits[3];
    } else if (null_dec(value_2)) {
        check = 3;
    } else {
        s21_decimal value1 = value_1;
        s21_decimal value2 = value_2;
        null_scale(&value1);
        null_scale(&value2);
        int count_shift = 0;
        s21_decimal minus_value2 = {0};
        reverse_for_div(&value1, &value2, &minus_value2, &count_shift);
        int count_shift_for_rem = count_shift;
        s21_decimal one = {{1, 0, 0, 0}};
        sumbits(minus_value2, one, &minus_value2);
        value1 = value_1;
        sumbits(value1, minus_value2, &value1);
        int left_bit = check_bit(value1, 95);
        left_bit ^= (1 << 0);
        s21_decimal result_buf = {{0, 0, 0, 0}};
        set_bit(&result_buf, 0, left_bit);
        shift_left(&value1, 1);
        div_process(&value1, value2, minus_value2, &result_buf, &count_shift, &left_bit);
        shift_right(&value1, 1);
        *result = value1;
        if (check_bit(*result, 95) == 1) {
        sumbits(*result, value2, result);
        }
        shift_right(result, count_shift_for_rem - count_shift);
    }
    if (check_scale(*result) > 28) {
        check = 2;
        result->bits[0] = 0;
        result->bits[1] = 0;
        result->bits[2] = 0;
        result->bits[3] = 0;
    }
    return check;
}

int div_process(s21_decimal *value1, s21_decimal value2,
                s21_decimal minus_value2, s21_decimal *result, int *count_shift, int *left_bit) {
  while (*count_shift > 0 && !null_dec(*value1)) {
    if (*left_bit == 1) {
      sumbits(*value1, minus_value2, value1);
    }
    if (*left_bit == 0) {
      sumbits(*value1, value2, value1);
    }
    *left_bit = check_bit(*value1, 95);
    *left_bit ^= (1 << 0);
    shift_left(result, 1);
    set_bit(result, 0, *left_bit);
    shift_left(value1, 1);
    (*count_shift)--;
  }
  while (*count_shift > 0) {
    shift_left(result, 1);
    set_bit(result, 0, 0);
    (*count_shift)--;
  }
  return 0;
}


int reverse_for_div(s21_decimal *value, s21_decimal *value2,
                    s21_decimal *minus_value2, int *count_shift) {
  while (count_bit(*value) > count_bit(*value2)) {
    shift_left(value2, 1);
    (*count_shift)++;
  }
  *minus_value2 = *value2;
  reverse(minus_value2, 0);
  reverse(minus_value2, 1);
  reverse(minus_value2, 2);
  return 0;
}

int count_bit(s21_decimal value) {
  int count = 96;
  while (check_bit(value, count - 1) != 1 && count) {
    count--;
  }
  return count;
}
