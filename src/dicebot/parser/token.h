#pragma once

#include <cstdint>
#include <deque>
#include <map>
#include <string>
#include <vector>

namespace dicebot::diceparser {

    enum class dice_macro_class_t { const_macro, rand_macro, dicelet_macro };

    enum class token_index : uint8_t {
        index_stop,
        index_begin,
        index_number,
        index_macro,
        ambiguity_indentifier,
        ambiguity_macro_d,
        keyword_d,
        keyword_k,
        keyword_kl,
        punct_add,
        punct_sub,
        punct_mul,
        punct_dvi,
        punct_lbrace,
        punct_rbrace,
        punct_comma,
        punct_lbracket,
        punct_rbracket,
        punct_sharp
    };

    constexpr char illegal_identifier[] = " +-*/{},()#=<>&|:%.";

    struct token_t {
        token_index id;
        size_t source_index;
        size_t pos_cur;
        size_t pos_next;
    };

    constexpr size_t npos = -1;

} // namespace dicebot::diceparser