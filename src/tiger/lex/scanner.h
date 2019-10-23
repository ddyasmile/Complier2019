#ifndef TIGER_LEX_SCANNER_H_
#define TIGER_LEX_SCANNER_H_

#include <algorithm>
#include <string>

#include "scannerbase.h"
#include "tiger/errormsg/errormsg.h"
#include "tiger/parse/parserbase.h"

extern EM::ErrorMsg errormsg;

class Scanner : public ScannerBase {
 public:
  explicit Scanner(std::istream &in = std::cin, std::ostream &out = std::cout);

  Scanner(std::string const &infile, std::string const &outfile);

  int lex();

 private:
  int lex__();
  int executeAction__(size_t ruleNr);

  void print();
  void preCode();
  void postCode(PostEnum__ type);
  void adjust();
  void adjustStr(char flag);

  int commentLevel_;
  std::string stringBuf_;
  int charPos_;
};

inline Scanner::Scanner(std::istream &in, std::ostream &out)
    : ScannerBase(in, out), charPos_(1) {}

inline Scanner::Scanner(std::string const &infile, std::string const &outfile)
    : ScannerBase(infile, outfile), charPos_(1) {}

inline int Scanner::lex() { return lex__(); }

inline void Scanner::preCode() {
  // optionally replace by your own code
}

inline void Scanner::postCode(PostEnum__ type) {
  // optionally replace by your own code
}

inline void Scanner::print() { print__(); }

inline void Scanner::adjust() {
  errormsg.tokPos = charPos_;
  charPos_ += length();
}

inline void Scanner::adjustStr(char flag) { 
  charPos_ += length();
  std::string tmp = matched();
  std::string str = "";
  switch (flag)
  {
  case 0:
    setMatched(stringBuf_);
    stringBuf_ = "";
    break;
  case 1:
    stringBuf_.append("\n");
    break;
  case 2:
    stringBuf_.append("\t");
    break;
  case 3:
    stringBuf_.append("\"");
    break;
  case 4:
    stringBuf_.append("\\");
    break;
  case 5:
    str += atoi(tmp.c_str()+1);
    stringBuf_.append(str);
    break;
  case 6:
    break;
  case 7:
    stringBuf_.append("\0");
    break;
  case 8:
    str += tmp[2]-'A'+1;
    stringBuf_.append(str);
    break;
  case 9: 
    stringBuf_.append(matched());
    break;
  default:
    break;
  }
}

#endif  // TIGER_LEX_SCANNER_H_

