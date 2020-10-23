#pragma once

#include <p4c/generator.h>

void p4c_build_output(const p4c_instruction_t* instructions, int instruction_count, const p4c_meta_data_t* meta_data, char* str, int str_sz);
