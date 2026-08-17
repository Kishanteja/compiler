#ifndef TOKENS_HPP
#define TOKENS_HPP

#include <vector>
#include <string>
#include <cstdio>

/* ════════════════════════════════════════════
   TOKEN BUFFER
   ════════════════════════════════════════════ */
struct TokenEntry {
    std::string name;
    std::string lexeme;
    int line;
};

inline std::vector<TokenEntry> token_buffer;

inline void buffer_token(const char *name, const char *lexeme, int line)
{
    token_buffer.push_back({ name, lexeme, line });
}

inline void print_toks(FILE *out)
{
    for (auto &t : token_buffer)
        fprintf(out, "\tToken name: %s \tLexeme: %s \t Lineno: %d\n",
                t.name.c_str(), t.lexeme.c_str(), t.line);
}

#endif