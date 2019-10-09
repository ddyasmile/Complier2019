#include "straightline/slp.h"
#include <iostream>

namespace A {
int A::CompoundStm::MaxArgs() const {
  // TODO: put your code here (lab1).
  int args1 = stm1->MaxArgs();
  int args2 = stm2->MaxArgs();

  return args1 > args2 ? args1 : args2;
}

Table *A::CompoundStm::Interp(Table *t) const {
  // TODO: put your code here (lab1).
  t = stm1->Interp(t);
  t = stm2->Interp(t);
  return t;
}

int A::AssignStm::MaxArgs() const {
  // TODO: put your code here (lab1).

  return exp->MaxArgs();
}

Table *A::AssignStm::Interp(Table *t) const {
  // TODO: put your code here (lab1).
  IntAndTable *intAndTable = exp->Interp(t);
  t = t->Update(id, intAndTable->i);
  free(intAndTable);
  return t;
}

int A::PrintStm::MaxArgs() const {
  // TODO: put your code here (lab1).

  return exps->MaxArgs();
}

Table *A::PrintStm::Interp(Table *t) const {
  // TODO: put your code here (lab1).
  IntAndTable *intAndTable = exps->Interp(t);
  t = intAndTable->t;
  free(intAndTable);
  return t;
}

int A::IdExp::MaxArgs() const {
  return 0;
}

IntAndTable *A::IdExp::Interp(Table *t) const {
  int value = t->Lookup(id);
  IntAndTable *intAndTable = new IntAndTable(value, t);
  return intAndTable;
}

int A::NumExp::MaxArgs() const {
  return 0;
}

IntAndTable *A::NumExp::Interp(Table *t) const {
  IntAndTable *intAndTable = new IntAndTable(num, t);
  return intAndTable;
}

int A::OpExp::MaxArgs() const {
  int args1 = left->MaxArgs();
  int args2 = right->MaxArgs();
  return args1 > args2 ? args1 : args2;
}

IntAndTable *A::OpExp::Interp(Table *t) const {
  IntAndTable *intAndTable1 = left->Interp(t);
  int leftValue = intAndTable1->i;
  t = intAndTable1->t;
  IntAndTable *intAndTable2 = right->Interp(t);
  int rightValue = intAndTable2->i;
  t = intAndTable2->t;
  free(intAndTable1);
  free(intAndTable2);
  int value;
  switch (oper) {
    case PLUS:
      value = leftValue + rightValue;
      break;
    case MINUS:
      value = leftValue - rightValue;
      break;
    case TIMES:
      value = leftValue * rightValue;
      break;
    case DIV:
      value = leftValue / rightValue;
      break;
    default:
      value = 0;
  }
  IntAndTable *intAndTable = new IntAndTable(value, t);
  return intAndTable;
}

int A::EseqExp::MaxArgs() const {
  int args1 = stm->MaxArgs();
  int args2 = exp->MaxArgs();
  return args1 > args2 ? args1 : args2;
}

IntAndTable *A::EseqExp::Interp(Table *t) const {
  t = stm->Interp(t);
  IntAndTable *intAndTable = exp->Interp(t);
  return intAndTable;
}

int A::PairExpList::MaxArgs() const {
  int args1 = head->MaxArgs();
  int args2 = tail->MaxArgs();
  int args3 = this->NumExps();
  if (args1>=args2&&args1>=args3)
    return args1;
  if (args2>=args1&&args2>=args3)
    return args2;
  return args3;
}

int A::PairExpList::NumExps() const {
  return 1 + tail->NumExps();
}

IntAndTable *A::PairExpList::Interp(Table *t) const {
  IntAndTable *intAndTable1 = head->Interp(t);
  t = intAndTable1->t;
  int value = intAndTable1->i;
  std::cout << value << " ";
  IntAndTable *intAndTable2 = tail->Interp(t);
  t = intAndTable2->t;
  free(intAndTable1);
  free(intAndTable2);
  IntAndTable *intAndTable = new IntAndTable(value, t);
  return intAndTable;
}

int A::LastExpList::MaxArgs() const {
  return last->MaxArgs();
}

int A::LastExpList::NumExps() const {
  return 1;
}

IntAndTable *A::LastExpList::Interp(Table *t) const {
  IntAndTable *intAndTable = last->Interp(t);
  std::cout << intAndTable->i << std::endl;
  return intAndTable;
}

int Table::Lookup(std::string key) const {
  if (id == key) {
    return value;
  } else if (tail != nullptr) {
    return tail->Lookup(key);
  } else {
    assert(false);
  }
}

Table *Table::Update(std::string key, int value) const {
  return new Table(key, value, this);
}
}  // namespace A
