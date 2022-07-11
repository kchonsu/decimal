#include "s21_decimal.h"

int s21_floor(s21_decimal value, s21_decimal *result) {
    int res = 1;
    if (check_scale(value) > 0) {
        s21_truncate(value, result);
        if (check_bit(*result, 127) == 1) {
            s21_decimal x = {{1, 0, 0, 0b10000000000000000000000000000000}};
            s21_add(*result, x, result);
        }
        res = 0;
    }
    return res;
}

int s21_round(s21_decimal value, s21_decimal *result) {
  int scale = check_scale(value);
  if (scale > 0) {
      s21_decimal rest = {{1, 0, 0, 0}};
      int div = 0;
      for (; scale > 0; scale--) {
          multen(&rest);
          div++;
      }
      s21_decimal result1 = {{0, 0, 0, 0}};
      null_scale(&value);
      int sign = get_sign(value);
      if (sign == 1) set_sign(&value, 0);
      s21_mod(value, rest, &result1);
      change_scale(&result1, div);
      change_scale(&value, div);
      while (div) {
          div_ten(&value);
          change_scale(&value, -1);
          div--;
      }
      if (sign == 1) set_sign(&value, 1);
      s21_decimal ten = {{10, 0, 0, 0}};
      s21_decimal one = {{1, 0, 0, 0}};
      int count = check_scale(result1);
      while (count > 1) {
          null_scale(&result1);
          s21_mod(result1, ten, &rest);
          div_ten(&result1);
          if (rest.bits[0] >= 5)  s21_add(result1, one, &result1);
          count--;
      }
      if (check_bit(value, 127) == 1) s21_negate(one, &one);
      if (result1.bits[0] >= 5) s21_add(value, one, &value);
  }
  result->bits[0] = value.bits[0];
  result->bits[1] = value.bits[1];
  result->bits[2] = value.bits[2];
  result->bits[3] = value.bits[3];
  return 0;
}

int s21_negate(s21_decimal value, s21_decimal *result) {
    int res = 0;
    int bit = check_bit(value, 127);
    if (bit == 1) set_bit(&value, 127, 0);
    else if (bit == 0)
        set_bit(&value, 127, 1);
    *result = value;
    return res;
}

int s21_truncate(s21_decimal value, s21_decimal *result) {
  int error = 0;
  if (check_scale(value) > 28) {
    error = 1;
  } else {
    *result = value;
    while (check_scale(*result) > 0) {
      error = div_ten(result);
      change_scale(result, -1);
    }
  }
  return error;
}
