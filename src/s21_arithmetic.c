#include "s21_decimal.h"

int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int error = 0;
  result->bits[0] = result->bits[1] = result->bits[2] = result->bits[3] = 0;
  long_decimal raw_result = {0};
  long_decimal raw_a = castDecToLong(value_1);
  long_decimal raw_b = castDecToLong(value_2);
  long_sum(raw_a, raw_b, &raw_result, LONGBITS, LONGSCALE);
  for (int i = 0; i < BITSVAL; i++) {
    if (long_check_bit(raw_b, i))
      error = long_sum(raw_a, raw_result, &raw_result, 0, LONGBITS);
    if (!error && !long_check_bit(raw_result, LONGBITS - 1)) {
      long_shift_left(&raw_a, 1);
    } else {
      error = 1;
      break;
    }
  }
  if (!error)
    error = castLongToDec(raw_result, result);
  if ((!result->bits[0] && !result->bits[1] && !result->bits[2]) || error) {
    result->bits[0] = result->bits[1] = result->bits[2] = result->bits[3] = 0;
  }
  return error;
}

int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int sign1 = get_sign(value_1);
  int sign2 = get_sign(value_2);
  int code = scale_dec(&value_1, &value_2);
  for (int i = 0; i < SCALEVAL; i++)
    set_bit(result, i, 0);
  if (code) {
    if (sign1 == sign2) {
      code = sumbits(value_1, value_2, result);
      set_sign(result, get_sign(value_1));
    } else {
      code = sub_bits(value_1, value_2, result);
    }
  }
  if ((!result->bits[0] && !result->bits[1] && !result->bits[2]) || code )
    result->bits[0] = result->bits[1] = result->bits[2] = result->bits[3] = 0;
  return code;
}

int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int code;
  int sign1 = get_sign(value_1), sign2 = get_sign(value_2);
  if ((sign1 && !sign2) || (!sign1 && sign2)) {
    set_sign(&value_1, 0);
    set_sign(&value_2, 0);
    code = s21_add(value_1, value_2, result);
    set_sign(result, sign1);
  } else {
    code = sub_bits(value_1, value_2, result);
    if (is_less_abs(value_1, value_2)) {
      if (!sign1)
        set_sign(result, 1);
      else
        set_sign(result, 0);
    } else {
      set_sign(result, get_sign(value_1) );
    }
  }
  if (!result->bits[0] && !result->bits[1] && !result->bits[2]) {
    result->bits[0] = result->bits[1] = result->bits[2] = result->bits[3] = 0;
  }
  return code;
}
