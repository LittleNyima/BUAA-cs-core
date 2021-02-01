#ifndef UTIL_H
#define UTIL_H

#include <string>
using namespace std;

bool is_digit(char);
bool is_blank(char);
bool is_letter(char);
bool is_character(char);
bool is_legal_ascii(char);
bool is_operator(char);
bool starts_with(string, string);
bool contains(string, char);
bool is_2_pow(int);

int log2_(int);

string to_lowercase(string);
string slice(string, int, int);
string slice(string, int);

#endif // !UTIL_H
