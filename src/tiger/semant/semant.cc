#include "tiger/semant/semant.h"
#include "tiger/errormsg/errormsg.h"

extern EM::ErrorMsg errormsg;

using VEnvType = S::Table<E::EnvEntry> *;
using TEnvType = S::Table<TY::Ty> *;

namespace {
static TY::TyList *make_formal_tylist(TEnvType tenv, A::FieldList *params) {
  if (params == nullptr) {
    return nullptr;
  }

  TY::Ty *ty = tenv->Look(params->head->typ);
  if (ty == nullptr) {
    errormsg.Error(params->head->pos, "undefined type %s",
                   params->head->typ->Name().c_str());
  }

  return new TY::TyList(ty->ActualTy(), make_formal_tylist(tenv, params->tail));
}

static TY::FieldList *make_fieldlist(TEnvType tenv, A::FieldList *fields) {
  if (fields == nullptr) {
    return nullptr;
  }

  TY::Ty *ty = tenv->Look(fields->head->typ);
  if (ty == nullptr)
  {
    errormsg.Error(fields->head->pos, "undefined type %s",
                   fields->head->typ->Name().c_str());
  }

  return new TY::FieldList(new TY::Field(fields->head->name, ty),
                           make_fieldlist(tenv, fields->tail));
}

}  // namespace

namespace A {

TY::Ty *SimpleVar::SemAnalyze(VEnvType venv, TEnvType tenv,
                              int labelcount) const {
  // TODO: Put your codes here (lab4).
  //errormsg.Error(this->pos, "simpleVar");

  S::Symbol *sym = this->sym;
  E::EnvEntry *envEnty = venv->Look(sym);
  if (envEnty && envEnty->kind == E::EnvEntry::Kind::VAR) {
    E::VarEntry *varEntry = (E::VarEntry *)envEnty;
    return varEntry->ty;
  }else {
    errormsg.Error(this->pos, "undefined variable %s", sym->Name().c_str());
    return TY::VoidTy::Instance();
  }
}

TY::Ty *FieldVar::SemAnalyze(VEnvType venv, TEnvType tenv,
                             int labelcount) const {
  // TODO: Put your codes here (lab4).
  //errormsg.Error(this->pos, "fieldVar");

  A::Var *var = this->var;
  S::Symbol *sym = this->sym;
  TY::Ty *ty = var->SemAnalyze(venv, tenv, labelcount)->ActualTy();

  //check var
  if (ty->kind != TY::Ty::Kind::RECORD) {
    errormsg.Error(this->pos, "not a record type");
    return TY::VoidTy::Instance();
  }

  //check field
  TY::FieldList *fields = ((TY::RecordTy *)ty)->fields;
  while (fields) {
    TY::Field *field = fields->head;
    if (field->name == sym) {
      return field->ty;
    }
    fields = fields->tail;
  }

  errormsg.Error(this->pos, "field %s doesn't exist", sym->Name().c_str());
  return TY::VoidTy::Instance();
}

TY::Ty *SubscriptVar::SemAnalyze(VEnvType venv, TEnvType tenv,
                                 int labelcount) const {
  // TODO: Put your codes here (lab4).
  //errormsg.Error(this->pos, "subscriptVar");
  A::Var *var = this->var;
  A::Exp *exp = this->subscript;
  TY::Ty *var_ty = var->SemAnalyze(venv,tenv,labelcount);

  //check var
  if (var_ty->kind != TY::Ty::Kind::ARRAY) {
    errormsg.Error(this->pos, "array type required");
    return TY::VoidTy::Instance();
  }

  //check exp
  TY::Ty *exp_ty = exp->SemAnalyze(venv,tenv,labelcount);
  if (exp_ty->kind != TY::Ty::Kind::INT) {
    errormsg.Error(this->pos, "array index must be integer");
    return TY::VoidTy::Instance();
  }

  TY::ArrayTy *ty = (TY::ArrayTy *)var_ty;
  return ty->ty;
}

TY::Ty *VarExp::SemAnalyze(VEnvType venv, TEnvType tenv, int labelcount) const {
  // TODO: Put your codes here (lab4).
  //errormsg.Error(this->pos, "Varexp");
  TY::Ty *var_type = this->var->SemAnalyze(venv, tenv, labelcount)->ActualTy();
  return var_type;
}

TY::Ty *NilExp::SemAnalyze(VEnvType venv, TEnvType tenv, int labelcount) const {
  // TODO: Put your codes here (lab4).
  //errormsg.Error(this->pos, "Nilexp");
  return TY::NilTy::Instance();
}

TY::Ty *IntExp::SemAnalyze(VEnvType venv, TEnvType tenv, int labelcount) const {
  // TODO: Put your codes here (lab4).
  //errormsg.Error(this->pos, "Intexp");
  return TY::IntTy::Instance();
}

TY::Ty *StringExp::SemAnalyze(VEnvType venv, TEnvType tenv,
                              int labelcount) const {
  // TODO: Put your codes here (lab4).
  //errormsg.Error(this->pos, "stringexp");
  return TY::StringTy::Instance();
}

TY::Ty *CallExp::SemAnalyze(VEnvType venv, TEnvType tenv,
                            int labelcount) const {
  // TODO: Put your codes here (lab4).
  //errormsg.Error(this->pos, "callexp");
  S::Symbol *func = this->func;
  E::EnvEntry *envEntry = venv->Look(func);

  //check func 
  if (!envEntry || envEntry->kind != E::EnvEntry::Kind::FUN) {
    errormsg.Error(this->pos, "undefined function %s", func->Name().c_str());
    return TY::VoidTy::Instance();
  }

  A::ExpList *args = this->args;
  E::FunEntry *funEntry = (E::FunEntry *)envEntry;
  TY::TyList *formals = funEntry->formals;

  //compare args types and tylist
  A::Exp *arg;
  TY::Ty *formal;
  for(; args&&formals; args = args->tail, formals = formals->tail) {
    if (!args) {
      errormsg.Error(this->pos, "too little params in function %s", func->Name().c_str());
      break;;
    }

    arg = args->head;
    formal = formals->head;
    TY::Ty *arg_ty = arg->SemAnalyze(venv,tenv,labelcount);

    if (!arg_ty->IsSameType(formal)) {
      //fprintf(stderr, "kind %d %d", formal->kind, arg_ty->kind);
      errormsg.Error(this->pos, "para type mismatch");
    }
  }

  if (args) {
    errormsg.Error(this->pos, "too many params in function %s", func->Name().c_str());
  }

  //fprintf(stderr, "result kind %d\n", funEntry->result->ActualTy()->kind);
  return funEntry->result;
}

TY::Ty *OpExp::SemAnalyze(VEnvType venv, TEnvType tenv, int labelcount) const {
  // TODO: Put your codes here (lab4).
  //errormsg.Error(this->pos, "opexp");
  A::Oper oper = this->oper;
  TY::Ty *left = this->left->SemAnalyze(venv,tenv,labelcount)->ActualTy();
  TY::Ty *right = this->right->SemAnalyze(venv,tenv,labelcount)->ActualTy();

  switch (oper)
  {
    //ALU operation
    case A::Oper::PLUS_OP: case A::Oper::MINUS_OP: 
    case A::Oper::TIMES_OP: case A::Oper::DIVIDE_OP:
      if (left->kind != TY::Ty::Kind::INT) {
        errormsg.Error(this->left->pos, "integer required");
      }
      if (right->kind != TY::Ty::Kind::INT) {
        errormsg.Error(this->right->pos, "integer required");
      }
      break;

    //compare operation
    case A::Oper::LT_OP: case A::Oper::LE_OP:
    case A::Oper::GT_OP: case A::Oper::GE_OP:
    case A::Oper::EQ_OP: case A::Oper::NEQ_OP:
      if (left->kind!=TY::Ty::Kind::INT&&left->kind!=TY::Ty::Kind::STRING) {
        errormsg.Error(this->left->pos, "integer or string required");
      } 
      if (right->kind!=TY::Ty::Kind::INT&&right->kind!=TY::Ty::Kind::STRING) {
        errormsg.Error(this->right->pos, "integer or string required");
      }
      if (!left->IsSameType(right)) {
        errormsg.Error(this->pos, "same type required");
      }
      break;
  }
  return TY::IntTy::Instance();
}

TY::Ty *RecordExp::SemAnalyze(VEnvType venv, TEnvType tenv,
                              int labelcount) const {
  // TODO: Put your codes here (lab4).
  //errormsg.Error(this->pos, "recordexp");
  S::Symbol *typ = this->typ;
  TY::Ty *typ_ty = tenv->Look(typ);

  if (!typ_ty) {
    errormsg.Error(this->pos, "undefined type %s", typ->Name().c_str());
    return TY::VoidTy::Instance();  
  }

//  TY::Ty *ty = typ_ty->ActualTy();
//  if (!ty) {
//    errormsg.Error(this->pos, "undefined type %s", typ->Name().c_str());
//    return TY::VoidTy::Instance();
//  }
//
//  if (ty->kind != TY::Ty::Kind::RECORD) {
//    errormsg.Error(this->pos, "not record type %s", typ->Name().c_str());
//    return TY::VoidTy::Instance();  
//  }
//
//  A::EFieldList *fields = this->fields;
//  TY::RecordTy *recordTy = (TY::RecordTy *)ty;
//  TY::FieldList *records = recordTy->fields;
//
//  //compare fields and record, include name and type
//  while (fields&&records)
//  {
//    A::EField *field = fields->head;
//    TY::Ty *field_ty = field->exp->SemAnalyze(venv,tenv,labelcount);
//
//    TY::Field *record = records->head;
//
//    if (field->name != record->name) {
//      errormsg.Error(this->pos, "field %s not defined", field->name->Name().c_str());
//      return TY::VoidTy::Instance();
//    }
//
//    if (!field_ty->IsSameType(record->ty)) {
//      errormsg.Error(this->pos, "field type mismatch");
//      return TY::VoidTy::Instance();
//    }
//
//    fields = fields->tail;
//    records = records->tail;
//  }
//
//  if (fields || records) {
//    errormsg.Error(this->pos, "field amount mismatch");
//  }
  
  return typ_ty;
}

TY::Ty *SeqExp::SemAnalyze(VEnvType venv, TEnvType tenv, int labelcount) const {
  // TODO: Put your codes here (lab4).
  //errormsg.Error(this->pos, "seqexp");
  A::ExpList *seq = this->seq;
  if (!seq) return TY::VoidTy::Instance();

  TY::Ty *ty;
  do {
    A::Exp *exp = seq->head;
    ty = exp->SemAnalyze(venv,tenv,labelcount);
  }while(seq = seq->tail);

  //return type of last exp
  return ty;
}

TY::Ty *AssignExp::SemAnalyze(VEnvType venv, TEnvType tenv,
                              int labelcount) const {
  // TODO: Put your codes here (lab4).
  //errormsg.Error(this->pos, "assignexp");
  A::Var *var = this->var;
  A::Exp *exp = this->exp;

  if (var->kind == A::Var::Kind::SIMPLE) {
    A::SimpleVar *simpleVar = (A::SimpleVar *)var;
    E::EnvEntry *envEntry = venv->Look(simpleVar->sym);
    if (!envEntry) {
      errormsg.Error(this->pos, "undefined type %s", simpleVar->sym->Name().c_str());
    }
    if (envEntry->kind!=E::EnvEntry::Kind::VAR) {
      errormsg.Error(this->pos, "%s is a function", simpleVar->sym->Name().c_str());
    }
    if (envEntry->readonly) {
      errormsg.Error(this->pos, "loop variable can't be assigned");
    }
  }

  TY::Ty *var_ty = var->SemAnalyze(venv,tenv,labelcount);
  TY::Ty *exp_ty = exp->SemAnalyze(venv,tenv,labelcount);
  if (!var_ty->IsSameType(exp_ty)) {
    //fprintf(stderr, "a b %d %d\n", var_ty->kind, exp_ty->kind);
    errormsg.Error(this->pos, "unmatched assign exp");
  }

  //return var_ty->ActualTy();
  return var_ty;
}

TY::Ty *IfExp::SemAnalyze(VEnvType venv, TEnvType tenv, int labelcount) const {
  // TODO: Put your codes here (lab4).
  //errormsg.Error(this->pos, "ifexp");
  A::Exp *test = this->test;
  A::Exp *then = this->then;
  A::Exp *elsee = this->elsee;

  TY::Ty *test_ty = test->SemAnalyze(venv,tenv,labelcount);
  TY::Ty *then_ty = then->SemAnalyze(venv,tenv,labelcount);

  if (test_ty->kind != TY::Ty::Kind::INT) {
    errormsg.Error(test->pos, "integer required");
  } 

  if (elsee&&elsee->kind != A::Exp::Kind::NIL) {
    TY::Ty *elsee_ty = elsee->SemAnalyze(venv,tenv,labelcount);
    if (!elsee_ty->IsSameType(then_ty)) {
      errormsg.Error(this->pos, "then exp and else exp type mismatch");
    }
  }else{
    //fprintf(stderr, "thenty kind %d\n", then_ty->kind);
    if (then_ty->kind != TY::Ty::Kind::VOID) {
      errormsg.Error(then->pos, "if-then exp's body must produce no value");
    }
  }

  return then_ty;
}

TY::Ty *WhileExp::SemAnalyze(VEnvType venv, TEnvType tenv,
                             int labelcount) const {
  // TODO: Put your codes here (lab4).
  //errormsg.Error(this->pos, "whileexp");
  A::Exp *test = this->test;
  A::Exp *body = this->body;

  TY::Ty *test_ty = test->SemAnalyze(venv,tenv,labelcount);
  TY::Ty *body_ty = body->SemAnalyze(venv,tenv,labelcount);

  if (test->kind != A::Exp::Kind::INT)
  {
    errormsg.Error(this->pos, "integer required");
  }
  if (body->kind != A::Exp::Kind::VOID)
  {
    errormsg.Error(this->pos, "while body must produce no value");
  }


  return body_ty;
}

TY::Ty *ForExp::SemAnalyze(VEnvType venv, TEnvType tenv, int labelcount) const {
  // TODO: Put your codes here (lab4).
  //errormsg.Error(this->pos, "forexp");
  S::Symbol *var = this->var;
  A::Exp *lo = this->lo;
  A::Exp *hi = this->hi;
  A::Exp *body = this->body;

  TY::Ty *lo_ty = lo->SemAnalyze(venv,tenv,labelcount);
  TY::Ty *hi_ty = hi->SemAnalyze(venv,tenv,labelcount);

  if (!lo_ty->IsSameType(hi_ty)||lo_ty->kind != TY::Ty::Kind::INT) {
    errormsg.Error(this->pos, "for exp's range type is not integer");
  }

  venv->BeginScope();
  tenv->BeginScope();

  E::VarEntry *varEntry = new E::VarEntry(lo_ty, true);
  venv->Enter(var, varEntry);
  TY::Ty *body_ty = body->SemAnalyze(venv,tenv,labelcount);

  venv->EndScope();
  tenv->EndScope();

  return body_ty;
}

TY::Ty *BreakExp::SemAnalyze(VEnvType venv, TEnvType tenv,
                             int labelcount) const {
  // TODO: Put your codes here (lab4).
  //errormsg.Error(this->pos, "breakexp");
  return TY::VoidTy::Instance();
}

TY::Ty *LetExp::SemAnalyze(VEnvType venv, TEnvType tenv, int labelcount) const {
  // TODO: Put your codes here (lab4).
  //errormsg.Error(this->pos, "letexp");
  venv->BeginScope();
  tenv->BeginScope();

  A::DecList *decs;
  for (decs = this->decs; decs; decs = decs->tail) {
    decs->head->SemAnalyze(venv,tenv,labelcount);
  }

  TY::Ty *ty = this->body->SemAnalyze(venv,tenv,labelcount);

  venv->EndScope();
  tenv->EndScope();

  return ty;
}

TY::Ty *ArrayExp::SemAnalyze(VEnvType venv, TEnvType tenv,
                             int labelcount) const {
  // TODO: Put your codes here (lab4).
  //errormsg.Error(this->pos, "arrayexp");
  S::Symbol *typ = this->typ;
  A::Exp *size = this->size;
  A::Exp *init = this->init;

  TY::Ty *typ_ty = tenv->Look(typ);
  if (!typ_ty) {
    errormsg.Error(this->pos, "undefined type %s", typ->Name().c_str());
    return TY::VoidTy::Instance();  
  }

  TY::Ty *ty = typ_ty->ActualTy();
  if (!ty) {
    errormsg.Error(this->pos, "undefined type %s", typ->Name().c_str());
    return TY::VoidTy::Instance();  
  }

  if (ty->kind != TY::Ty::Kind::ARRAY) {
    errormsg.Error(this->pos,  "not array type");
    return TY::VoidTy::Instance();  
  }

  TY::Ty *size_ty = size->SemAnalyze(venv,tenv,labelcount);
  TY::Ty *init_ty = init->SemAnalyze(venv,tenv,labelcount);

  TY::ArrayTy *arrayTy = (TY::ArrayTy *)ty;
  if (!init_ty->IsSameType(arrayTy->ty)) {
    errormsg.Error(init->pos, "init exp type mismatch");
  }

  if (size_ty->kind != TY::Ty::Kind::INT) {
    errormsg.Error(size->pos, "size integer required");
  }
  
  return ty;
}

TY::Ty *VoidExp::SemAnalyze(VEnvType venv, TEnvType tenv,
                            int labelcount) const {
  // TODO: Put your codes here (lab4).
  //errormsg.Error(this->pos, "voidexp");
  return TY::VoidTy::Instance();
}

void FunctionDec::SemAnalyze(VEnvType venv, TEnvType tenv,
                             int labelcount) const {
  // TODO: Put your codes here (lab4).
  //errormsg.Error(this->pos, "functiondec");
  A::FunDecList *functions = this->functions;

  //put function decs to env
  do {
    //function name
    A::FunDec *function = functions->head;
    S::Symbol *name = function->name;

    if (venv->Look(name)) {
      errormsg.Error(function->pos, "two functions have the same name");
      continue;
    }

    //result
    TY::Ty *result_ty;
    S::Symbol *result = function->result;
    TY::TyList *formals = make_formal_tylist(tenv, function->params);

    if (result) {
      result_ty = tenv->Look(result);
      if (result_ty->kind==TY::Ty::Kind::VOID) {
        errormsg.Error(function->pos, "undefined result type %s", result->Name().c_str());
        continue;
      }
      venv->Enter(function->name, new E::FunEntry(formals, result_ty));
    }else{
      venv->Enter(function->name, new E::FunEntry(formals, TY::VoidTy::Instance()));
    }
  }while(functions = functions->tail);

  //check function body
  functions = this->functions;

  do {
    A::FunDec *function = functions->head;
    E::EnvEntry *envEntry = venv->Look(function->name);

    if (!envEntry) {
      errormsg.Error(function->pos, "undefined function %s", function->name->Name().c_str());
      continue;
    }

    if (envEntry->kind != E::EnvEntry::Kind::FUN) {
      errormsg.Error(function->pos, "%s is not a function", function->name->Name().c_str());
      continue;
    }
    
    //local variable
    A::FieldList *params = function->params;
    E::FunEntry *funEntry = (E::FunEntry *)envEntry;
    TY::TyList *formals = funEntry->formals;

    venv->BeginScope();
    tenv->BeginScope();

    for (; params&&formals; params=params->tail, formals=formals->tail) {
      venv->Enter(params->head->name, new E::VarEntry(formals->head));
    } 

    //result and body type
    TY::Ty *result = funEntry->result;
    TY::Ty *body_ty = function->body->SemAnalyze(venv,tenv,labelcount);

    

    if (!result->IsSameType(body_ty)) {
      if (result->kind==TY::Ty::Kind::VOID) {
        errormsg.Error(function->body->pos, "procedure returns value");
      }else{
        errormsg.Error(function->body->pos, "return type mismatch");
      }
    }

    venv->EndScope();
    tenv->EndScope();
  }while (functions = functions->tail);
}

void VarDec::SemAnalyze(VEnvType venv, TEnvType tenv, int labelcount) const {
  // TODO: Put your codes here (lab4).
  //errormsg.Error(this->pos, "vardec");
  S::Symbol *var = this->var;
  S::Symbol *typ = this->typ;
  A::Exp *init = this->init;

  TY::Ty *init_ty = init->SemAnalyze(venv,tenv,labelcount);

  if (venv->Look(var)) {
    errormsg.Error(this->pos, "two variables have the same name");
    return;
  }

  if (!typ) {
    if (init_ty->kind == TY::Ty::Kind::NIL) {
      errormsg.Error(init->pos, "init should not be nil without type specified");
      return;
    }
    venv->Enter(this->var, new E::VarEntry(init_ty));
    return;
  }

  TY::Ty *typ_ty = tenv->Look(typ);
  if (!typ_ty) {
    errormsg.Error(this->pos, "undefined type %s", typ->Name().c_str());
    return;
  }

  if (init_ty->kind == TY::Ty::Kind::NIL&&typ_ty->ActualTy()->kind != TY::Ty::Kind::RECORD) {
    errormsg.Error(this->pos, "init should not be nil without type specified");
  }

  if (!typ_ty->IsSameType(init_ty)&&init_ty->kind != TY::Ty::Kind::NIL) {
    errormsg.Error(this->pos, "type mismatch");
  }
  //fprintf(stderr, "initty %d\n", init_ty->kind);
  venv->Enter(var, new E::VarEntry(init_ty));
}

void TypeDec::SemAnalyze(VEnvType venv, TEnvType tenv, int labelcount) const {
  // TODO: Put your codes here (lab4).
  //errormsg.Error(this->pos, "typedec");
  A::NameAndTyList *types = this->types;
  A::NameAndTyList *cur_type = types;

  //add name to tenv
  do {
    A::NameAndTy *head = cur_type->head;
    S::Symbol *name = head->name;

    if (tenv->Look(name)) {
      errormsg.Error(this->pos, "two types have the same name");
      continue;
    }

    tenv->Enter(name, new TY::NameTy(name, nullptr));
  }while(cur_type = cur_type->tail);

  //bind name with type
  cur_type = types;
  do {
    TY::Ty *ty = tenv->Look(cur_type->head->name);
    ((TY::NameTy *)ty)->ty = cur_type->head->ty->SemAnalyze(tenv);
  }while(cur_type = cur_type->tail);

  cur_type = types;
  bool noCycle = true;
  do {
    A::NameAndTy *head = cur_type->head;
    TY::Ty *ty = tenv->Look(head->name);
    if (ty->kind ==TY::Ty::Kind::NAME) {
      TY::Ty *ty_ty = ((TY::NameTy *)ty)->ty;
      while (ty_ty->kind == TY::Ty::Kind::NAME)
      {
        TY::NameTy *nameTy = (TY::NameTy *)ty_ty;
        if (!nameTy->sym->Name().compare(cur_type->head->name->Name())) {
          errormsg.Error(pos, "illegal type cycle");
          noCycle = false;
          break;
        }
        ty_ty = nameTy->ty;
      }
    }
  }while((cur_type = cur_type->tail)&&noCycle);
}

TY::Ty *NameTy::SemAnalyze(TEnvType tenv) const {
  // TODO: Put your codes here (lab4).
  //errormsg.Error(this->pos, "namety");
  S::Symbol *name = this->name;
  TY::Ty *ty = tenv->Look(name);

  if (!ty) {
    errormsg.Error(this->pos, "undefined type %s", name->Name().c_str());
    return TY::VoidTy::Instance();  
  }

  return new TY::NameTy(name, ty);
}

TY::Ty *RecordTy::SemAnalyze(TEnvType tenv) const {
  // TODO: Put your codes here (lab4).
  //errormsg.Error(this->pos, "recordty");
  A::FieldList *fields = this->record;
  TY::FieldList *fields_ty = make_fieldlist(tenv, fields);

  return new TY::RecordTy(fields_ty);
}

TY::Ty *ArrayTy::SemAnalyze(TEnvType tenv) const {
  // TODO: Put your codes here (lab4).
  //errormsg.Error(this->pos, "arraytype");
  S::Symbol *array = this->array;
  TY::Ty *ty = tenv->Look(array);

  if (!ty) {
    errormsg.Error(this->pos, "undefined type %s", array->Name().c_str());
    return TY::VoidTy::Instance();  
  }

  return new TY::ArrayTy(ty);
}

}  // namespace A

namespace SEM {
void SemAnalyze(A::Exp *root) {
  if (root) root->SemAnalyze(E::BaseVEnv(), E::BaseTEnv(), 0);
}

}  // namespace SEM
