namespace lex {

// -------------------------------------------------------------------------- //
// Lexer control

// Advance the lexer by n characters assuming that a) the new position
// is not past the limit, and b) a newline sequence is not included
// in that string.
template<typename L>
  inline void
  advance(L& lex, int n) {
    lex.first += n;
    lex.loc.col += n;
  }

// Save a token having the given location, symbol, and text.
template<typename L>
  inline void
  save(L& lex, Token_kind k, String str) {
    lex.toks.emplace_back(lex.loc, k, str);
  }


// -------------------------------------------------------------------------- //
// Characters

//Returns true if consists of [a-zA-Z0-9_] and includes a dot '.'
template<typename L>
inline bool
is_module(L& lex) {
  if(is_id_head(*lex.first)) //File will start with valid id char (i.e., not a digit, parenthesis, etc...)
  {
    auto iter = lex.first + 1;
    while (iter != lex.last && is_file_rest(*iter)){
      if(*iter == '.') //File must contain at least one dot to define directory
        return true;
      ++iter;
    }
    return false;
  }
  else 
    return false;
}
// Returns true if c is in [a-zA-Z_].
inline bool
is_id_head(char c) { return std::isalpha(c) || c == '_'; }

// Returns true if c is in [a-zA-Z0-9_.]
inline bool
is_file_rest(char c) { return std::isalpha(c) || std::isdigit(c) || c == '.'; } 

// Returns true if c is in [a-zA-Z0-9_].
inline bool
is_id_rest(char c) {return std::isalpha(c) || std::isdigit(c) || c == '_'; }

// Returns true if c is in [0-9].
inline bool
is_digit(char c) { return std::isdigit(c); }

// Returns true if c is in [0-1].
inline bool
is_bin_digit(char c) { return c == '0' || c == '1'; }

// Returns true if c is in [0-9a-fA-F]
inline bool
is_hex_digit(char c) { return std::isxdigit(c); }

inline bool
is_newline(char c) { return c == '\n'; }


// -------------------------------------------------------------------------- //
// Lexing rules

// Returns true if the next character matches c.
template<typename L>
  inline bool
  next_char_is(L& lex, char c) {
    if (lex.last - lex.first > 1)
      return *(lex.first + 1) == c;
    else
      return false;
  }

// Consume horizontal whitespace starting at the current character.
template<typename L>
  inline void
  space(L& lex) { advance(lex); }

// Consume a newline starting at the current character.
//
// TODO: Recognize CR as a newline character and handle combinations of
// CR/LF and LF/CR as a single newline.
template<typename L>
  inline void
  newline(L& lex) {
    ++lex.first;
    ++lex.loc.line;
    lex.loc.col = 1;
  }

// Consume a comment, starting with "//" and up to (but not including)
// the new line.
template<typename L>
  inline void
  comment(L& lex) {
    lex.first += 2;
    while (lex.first != lex.last and *lex.first != '\n')
      ++lex.first;
  }

// Consume an n-character lexeme, creating a token.
template<typename L>
  inline void
  ngraph(L& lex, Token_kind sym, int n) {
    String str(lex.first, lex.first + n);
    save(lex, sym, str);
    advance(lex, n);
  }

// Consume a single-character symbol, creating a corresponding token.
template<typename L>
  inline void
  unigraph(L& lex, Token_kind k) { return ngraph(lex, k, 1); }

// Consume a 2-character symnbol, creating a token.
template<typename L>
  inline void
  digraph(L& lex, Token_kind k) { return ngraph(lex, k, 2); }

// Emit an error and advance past that character.
template<typename L>
  inline void
  error(L& lex) {
    ::error(lex.loc) << format("unrecognized character '{}'", *lex.first); 
    advance(lex);
  }

// Consume an identifier or keyword. The id argument defines
// the kind of token when it isn't a keyword.
template<typename L>
  inline void
  id(L& lex) {
    auto iter = lex.first + 1;
    while (iter != lex.last and is_id_rest(*iter))
      ++iter;

    // Build the token.
    String str(lex.first, iter);
    if (Token_kind k = keyword(str))
      save(lex, k, str);
    else
      save(lex, identifier_tok, str);
    advance(lex, iter - lex.first);
  }

  template<typename L>
  inline bool 
  is_import_line(L& lex)
  {
    auto iter = lex.first;
    while (iter != lex.last and !is_newline(*iter))
    {
      if(*iter == ';')
        return false; //semi-colon indicates a standard code line
      ++iter;
    }
    return true;
  }

  template<typename L>
  inline int 
  remaining_dots(L& lex)
  {
    int cnt = 0;
    auto iter = lex.first;
    while (iter != lex.last and is_file_rest(*iter))
    {
      if(*iter == '.')
        cnt++;
      ++iter;
    }
    return cnt;
  }
  
  // Consume a module reference. module extension
template<typename L>
  inline void
  module(L& lex) {
    auto iter = lex.first;
    do{
      if(*lex.first == '.')
        advance(lex, 1); //Move past preceding dot
      iter = lex.first;
      while (iter != lex.last and is_file_rest(*iter) and *iter != '.') //stop scanning when a '.' is found which defines directory/file/method
        ++iter;

      bool import_line = is_import_line(lex);
      int dots = remaining_dots(lex);
      String str(lex.first, iter);
        
      //If import line, there will not be a method/const reference.
      if((!import_line && dots > 1) || (import_line && dots == 1))
      {
        // Build the directory.
        save(lex, directory_tok, str);
        advance(lex, iter - lex.first);
      }
      else if((!import_line && dots == 1) || (import_line && dots == 0))
      {
        // Build the file
        save(lex, file_tok, str);
        advance(lex, iter - lex.first);
      }
    }
    while(*iter == '.');
  }

// Lex an integer.
//
// TODO: Support binary, octal, and hexadecimal literals.
template<typename L>
  inline void
  integer(L& lex) {
    auto iter = lex.first + 1;
    while (iter != lex.last and is_digit(*iter))
      ++iter;
    String str(lex.first, iter);
    save(lex, decimal_literal_tok, str);
    advance(lex, iter - lex.first);
  }

// Lex a string literal. A string literal is enclosed in quotes
// and may contain esacape characters. An escape character is
// a '\'' followed by a character.
// 
// TODO: Allow for extended forms of escape characters?
template<typename L>
  inline void
  string(L& lex) {
    auto iter = lex.first + 1;
    while (iter != lex.last && *iter != '"') {
      if (*iter == '\\')
        ++iter;
      ++iter;
    }
    ++iter; // Keep the enclosing quote.
    String str(lex.first, iter);
    save(lex, string_literal_tok, str);
    advance(lex, iter - lex.first);
  }

} // lex