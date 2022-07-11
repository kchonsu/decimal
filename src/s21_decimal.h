#ifndef SRC_S21_DECIMAL_H_
#define SRC_S21_DECIMAL_H_

#include <stdio.h>
#include <math.h>
#include <limits.h>

#define BITSVAL 96
#define SCALEVAL 128
#define LONGBITS 192
#define LONGSCALE 224

#define minimum 1e-28
#define int_max 2147483647
#define s21_INF 1.0 / 0.0
#define s21_NAN 0.0 / 0.0
#define uns_max 4294967295

typedef struct {
    int bits[4];
} s21_decimal;

typedef struct {
    int bits[7];
} long_decimal;

//  АРИФМЕТИЧЕСКИЕ ОПЕРАТОРЫ
int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_mod(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);

//  ФУНКЦИИ СРАВНЕНИЯ
int s21_is_equal(s21_decimal one, s21_decimal two);
int s21_is_not_equal(s21_decimal one, s21_decimal two);
int s21_is_less(s21_decimal one, s21_decimal two);
int s21_is_less_or_equal(s21_decimal one, s21_decimal two);
int s21_is_greater(s21_decimal one, s21_decimal two);
int s21_is_greater_or_equal(s21_decimal one, s21_decimal two);

//  ФУНКЦИИ КОНВЕРТАЦИИ
int s21_from_int_to_decimal(int src, s21_decimal *dst);
int s21_from_float_to_decimal(float src, s21_decimal *dst);
int s21_from_decimal_to_int(s21_decimal src, int *dst);
int s21_from_decimal_to_float(s21_decimal src, float *dst);

//  ФУНКЦИИ ОКРУГЛЕНИЯ И СМЕНЫ ЗНАКА
int s21_floor(s21_decimal value, s21_decimal *result);
int s21_round(s21_decimal value, s21_decimal *result);
int s21_truncate(s21_decimal value, s21_decimal *result);
int s21_negate(s21_decimal value, s21_decimal *result);

//  ДОПОЛНИТЕЛЬНЫЕ ФУНКЦИИ
int see_bit(s21_decimal x, int mode);
int zero_left(s21_decimal x);
int zero_right(s21_decimal x);
int get_sign(s21_decimal value);
void set_sign(s21_decimal *value, int sign);
int reverse(s21_decimal *x, int n);
int check_bit(s21_decimal x, int i);
int reverse_bit(s21_decimal *x, int i);
int set_bit(s21_decimal *x, int i, int k);
void shift_right(s21_decimal *value, int i);
void shift_left(s21_decimal *value, int i);
int check_scale(s21_decimal value);
void change_scale(s21_decimal *value, int change_scale);
int null_dec(s21_decimal x);
void null_scale(s21_decimal *x);
int sumbits(s21_decimal a, s21_decimal b, s21_decimal *result);
int scale_dec(s21_decimal *one_dec, s21_decimal *two_dec);
int multen(s21_decimal *val);
int div_ten(s21_decimal *value_1);
int div_process(s21_decimal *value1, s21_decimal value2,
                s21_decimal minus_value2, s21_decimal *result, int *count_shift, int *left_bit);
int reverse_for_div(s21_decimal *value, s21_decimal *value2,
                    s21_decimal *minus_value2, int *count_shift);
int div_correct(s21_decimal *value_1, s21_decimal *value_2, int *scale_dif);
int count_bit(s21_decimal value);
void plant_sign(s21_decimal one, s21_decimal two, int *result);
int less_or_great(s21_decimal one, s21_decimal two);

int is_less_abs(s21_decimal one, s21_decimal two);
int sub_bits(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);

int long_set_bit(long_decimal *x, int i, int k);
int long_check_bit(long_decimal x, int i);
int long_zero_left(long_decimal x);
int long_reverse(long_decimal *x, int n);
int overflow(long_decimal src);
int is_greater_ten(long_decimal value);
void long_shift_left(long_decimal *value, int i);
void long_shift_right(long_decimal *value, int i);
int long_check_scale(long_decimal value);
void long_change_scale(long_decimal *value, int change_scale);
int long_sum(long_decimal a, long_decimal b, long_decimal *result, int start, int end);
int long_div_ten(long_decimal *value_1);
int long_see_bit(long_decimal x, int mode);
long_decimal castDecToLong(s21_decimal src);
int castLongToDec(long_decimal src, s21_decimal *dst);
int long_truncate(long_decimal value, long_decimal *result);
int bank_sum(s21_decimal a, s21_decimal b, s21_decimal *result);

int get_float_exp(float *src);
int get_float_bit(float src);

#endif  // SRC_S21_DECIMAL_H_
