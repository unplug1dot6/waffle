
#ifndef AST_HPP
#define AST_HPP

#include "pretty.hpp"

#include "lang/string.hpp"
#include "lang/integer.hpp"
#include "lang/location.hpp"
#include "lang/nodes.hpp"

#include <iosfwd>
#include <map>

// -------------------------------------------------------------------------- //
// Language terms

// Name terms
constexpr Node_kind id_expr      = make_name_node(1);  // n
// Unit terms
constexpr Node_kind unit_term    = make_term_node(1);  // unit
// Boolean terms
constexpr Node_kind true_term    = make_term_node(10); // true
constexpr Node_kind false_term   = make_term_node(11); // false
constexpr Node_kind if_term      = make_term_node(12); // if t1 then t2 else t3
//Boolean operators
constexpr Node_kind and_term     = make_term_node(13); // t1 and t2
constexpr Node_kind or_term      = make_term_node(14); // t1 or t2
constexpr Node_kind not_term     = make_term_node(15); // t1 not t2
//Comparison operators
constexpr Node_kind equals_term  = make_term_node(16); // t1 == t2
constexpr Node_kind less_term    = make_term_node(17); // t1 < t2
// Numeric terms
constexpr Node_kind int_term     = make_term_node(20); // N
constexpr Node_kind succ_term    = make_term_node(21); // succ t
constexpr Node_kind pred_term    = make_term_node(22); // pred t
constexpr Node_kind iszero_term  = make_term_node(23); // iszero t
// String terms
constexpr Node_kind str_term    = make_term_node(25);  // "str"
// Lambda terms
constexpr Node_kind var_term     = make_term_node(30); // x : T
constexpr Node_kind abs_term     = make_term_node(31); // \v.t
constexpr Node_kind fn_term      = make_term_node(32); // \(v1, ..., vn).t
constexpr Node_kind app_term     = make_term_node(33); // t1 t2
constexpr Node_kind call_term    = make_term_node(34); // (t1, ..., tn)
// Tuples, records, and variants
constexpr Node_kind tuple_term   = make_term_node(40); // {t1, ..., tn}
constexpr Node_kind list_term    = make_term_node(41); // [t1, ..., tn]
constexpr Node_kind record_term  = make_term_node(42); // {l1=t1, ..., ln=tn}
constexpr Node_kind variant_term = make_term_node(43); // <l1=t1, ..., ln=tn>
constexpr Node_kind comma_term   = make_term_node(44); // t1, ..., tn
constexpr Node_kind proj_term    = make_term_node(45); // t1.n
constexpr Node_kind mem_term     = make_term_node(46); // t1.x
// Declarations
constexpr Node_kind def_term     = make_term_node(50); // def n = t
constexpr Node_kind init_term    = make_term_node(51); // n = t
// Tables, Table attributes, Relational Algebra
constexpr Node_kind table_term   = make_term_node(60); // {{x1:T1 = v:T1, x2:T2 = v:T2, ...}, ...}
constexpr Node_kind select_term  = make_term_node(61);
constexpr Node_kind join_on_term = make_term_node(62);
constexpr Node_kind union_term   = make_term_node(63); // t1 union t2
constexpr Node_kind intersect_term = make_term_node(64); // t1 intersect t2
constexpr Node_kind except_term  = make_term_node(65); // t1 except t2
constexpr Node_kind col_term     = make_term_node(66); // table.n (col proj)
// Module terms
constexpr Node_kind mod_id_term  = make_term_node(67);
// Miscellaneous terms
constexpr Node_kind ref_term     = make_term_node(100); // ref to decl
constexpr Node_kind print_term   = make_term_node(101); // print t
constexpr Node_kind prog_term    = make_term_node(500); // t1; ...; tn
// Types
constexpr Node_kind kind_type    = make_type_node(1);  // *
constexpr Node_kind unit_type    = make_type_node(2);  // Unit
constexpr Node_kind bool_type    = make_type_node(3);  // Bool
constexpr Node_kind nat_type     = make_type_node(5);  // Nat
constexpr Node_kind str_type     = make_type_node(6);  // Str
constexpr Node_kind arrow_type   = make_type_node(20); // T -> U
constexpr Node_kind fn_type      = make_type_node(21); // (T1, ..., Tn) -> U
constexpr Node_kind tuple_type   = make_type_node(22); // {T1, ..., Tn}
constexpr Node_kind list_type    = make_type_node(23); // [T]
constexpr Node_kind record_type  = make_type_node(24); // {l1:T1, ..., ln:Tn}
constexpr Node_kind variant_type = make_type_node(25); // <l1:T1, ..., ln:Tn>
constexpr Node_kind wild_type    = make_type_node(30); // *x:T


// -------------------------------------------------------------------------- //
// Abstract terms

struct Name;
struct Type;
struct Term;
struct Cond;

// Every distinct phrase in the language is an expression.
//
// The expression class also provides a facility for caching the type
// of the expression. This is generally assigned during elaboration or
// when nodes are initialized by default.
struct Expr : Node { 
  Expr(Node_kind k, Type* t) 
    : Node(k), tr(t) { }
  Expr(Node_kind k, const Location& l, Type* t) 
    : Node(k, l), tr(t) { }
  Type* tr;
};

// The base class of all identifiers in the language.
struct Name : Expr { using Expr::Expr; };

// The base class of all types in the language.
struct Type : Expr { using Expr::Expr; };

// The base class of all terms in the language.
struct Term : Expr { using Expr::Expr; };

// A sequence of expressions.
using Expr_seq = Seq<Expr>;

// A sequence of terms.
using Term_seq = Seq<Term>;

// A sequence of types.
using Type_seq = Seq<Type>;


// -------------------------------------------------------------------------- //
// Names
//
// Note that names may not have types.

// Represents the name of a declared entity in the language (e.g.,
// a function, variable, etc.).
struct Id : Name {
  Id(String n)
    : Name(id_expr, nullptr), t1(n) { }
  Id(const Location& l, String n)
    : Name(id_expr, l, nullptr), t1(n) { }

  String t1;
};

// -------------------------------------------------------------------------- //
// Id specific to modules
struct Mod_id : Name {
  Mod_id(String n)
    : Name(mod_id_term, nullptr), t1(n) { }

  String t1;
};

// -------------------------------------------------------------------------- //
// Terms

// The unit value.
struct Unit : Term {
  Unit(Type* t) 
    : Term(unit_term, t) { }
  Unit(const Location& l, Type* t) 
    : Term(unit_term, l, t) { }
};

// Represents the constant term 'true'.
struct True : Term {
  True(Type* t) 
    : Term(true_term, t) { }
  True(const Location& l, Type* t) 
    : Term(true_term, l, t) { }
};

// Represents the constant term 'false'.
struct False : Term {
  False(Type* t) 
    : Term(false_term, t) { }
  False(const Location& l, Type* t) 
    : Term(false_term, l, t) { }
};

// Represents the conditional term 'if t1 then t2 else t3'.
struct If : Term {
  If(Type* t0, Term* t1, Term* t2, Term* t3) 
    : Term(if_term, t0), t1(t1),  t2(t2), t3(t3) { }
  If(const Location& l, Type* t, Term* t1, Term* t2, Term* t3) 
    : Term(if_term, l, t), t1(t1), t2(t2), t3(t3) { }

  Term* cond() const { return t1; }
  Term* if_true() const { return t2; }
  Term* if_false() const { return t3; }

  Term* t1;
  Term* t2;
  Term* t3;
};

// Represents an integer literal.
struct Int : Term {
  Int(Type* t, const Integer& n) 
    : Term(int_term, t), t1(n) { }
  Int(const Location& l, Type* t, const Integer& n) 
    : Term(int_term, l, t), t1(n) { }

  const Integer& value() const { return t1; }

  Integer t1;
};

// Represents the boolean operator term t1 AND t2
// t1 or t2 has to be type bool
struct And :Term {
  And(Type* t, Term* t1, Term* t2)
    : Term(and_term, t), t1(t1), t2(t2) { }
  And(const Location& l, Type* t, Term* t1, Term* t2)
    : Term(and_term, l, t), t1(t1), t2(t2) { }

  Term* t1;
  Term* t2;
};

// Represents the boolean operator term t1 OR t2
// t1 or t2 has to be of type bool
struct Or :Term {
  Or(Type* t, Term* t1, Term* t2)
    : Term(or_term, t), t1(t1), t2(t2) { }
  Or(const Location& l, Type* t, Term* t1, Term* t2)
    : Term(or_term, l, t), t1(t1), t2(t2) { }

  Term* t1;
  Term* t2;
};

// Represents the boolean operator term not t1
// t1 or t2 has to be of type bool
struct Not :Term {
  Not(Type* t, Term* t1)
    : Term(not_term, t), t1(t1){ }
  Not(const Location& l, Type* t, Term* t1)
    : Term(not_term, l, t), t1(t1) { }

  Term* t1;
};

// Represents the comparison operator term t1 == t2
struct Equals :Term {
  Equals(Type* t, Term* t1, Term* t2)
    : Term(equals_term, t), t1(t1), t2(t2) { }
  Equals(const Location& l, Type* t, Term* t1, Term* t2)
    : Term(equals_term, l, t), t1(t1), t2(t2) { }

  Term* t1;
  Term* t2;
};

// Represents the comparison operator term t1 < t2
struct Less :Term {
  Less(Type* t, Term* t1, Term* t2)
    : Term(less_term, t), t1(t1), t2(t2) { }
  Less(const Location& l, Type* t, Term* t1, Term* t2)
    : Term(less_term, l, t), t1(t1), t2(t2) { }

  Term* t1;
  Term* t2;
};

// Represents the term 'succ t'.
struct Succ : Term {
  Succ(Type* t0, Term* t) 
    : Term(succ_term, t0), t1(t) { }
  Succ(const Location& l, Type* t0, Term* t) 
    : Term(succ_term, l, t0), t1(t) { }

  Term* arg() const { return t1; }

  Term* t1;
};

// Represents the term 'pred t'.
struct Pred : Term {
  Pred(Type* t0, Term* t) 
    : Term(pred_term, t0), t1(t) { }
  Pred(const Location& l, Type* t0, Term* t) 
    : Term(pred_term, l, t0), t1(t) { }

  Term* arg() const { return t1; }

  Term* t1;
};

// Represents the term 'iszero t'.
struct Iszero : Term {
  Iszero(Type* t0, Term* t) 
    : Term(iszero_term, t0), t1(t) { }
  Iszero(const Location& l, Type* t0, Term* t) 
    : Term(iszero_term, l, t0), t1(t) { }

  Term* arg() const { return t1; }

  Term* t1;
};

// Represents the string literal "...", a sequence of characters
// enclosed in quotes.
struct Str : Term {
  Str(Type* t, String s)
    : Term(str_term, t), t1(s) { }
  Str(const Location& l, Type* t, String s)
    : Term(str_term, l, t), t1(s) { }

  String value() const { return t1; }

  String t1;
};

// A variable declaration of the form 'x : T' in a lambda
// abstraction. 
struct Var : Term {
  Var(Name* n, Type* t) 
    : Term(var_term, t), t1(n), t2(t) { }
  Var(const Location& l, Name* n, Type* t) 
    : Term(var_term, l, t), t1(n), t2(t) { }

  Name* name() const { return t1; }
  Type* type() const { return t2; }

  Name* t1;
  Type* t2;
};

// A lambda abstraction over a term, having the form '\v.t' where 'v' 
// is a variable declaration and 't' is the abstracted term.
struct Abs : Term {
  Abs(Type* t0, Term* x, Term* t)
    : Term(abs_term, t0), t1(x), t2(t) { }
  Abs(const Location& l, Type* t0, Term* x, Term* t) 
    : Term(abs_term, l, t0), t1(x), t2(t) { }

  Term* var() const { return t1; }
  Term* term() const { return t2; }

  Term* t1;
  Term* t2;
};

// A function of the form '\(v1, ..., vn).t' where 'vi' is a
// variable declaration and 't' is the abstracted term. Unlike
// an abstraction, a function can be called with many arguments.
struct Fn : Term {
  Fn(Type* t0, Term_seq* ps, Term* t)
    : Term(fn_term, t0), t1(ps), t2(t) { }
  Fn(const Location& l, Type* t0, Term_seq* ps, Term* t) 
    : Term(fn_term, l, t0), t1(ps), t2(t) { }

  Term_seq* parms() const { return t1; }
  Term* term() const { return t2; }

  Term_seq* t1;
  Term* t2;
};

// An application of an abstraction to a term, having the form 't1 t2' 
// where 't1' is the abstraction and 't2' is the argument.
struct App : Term {
  App(Type* t0, Term* t1, Term* t2) 
    : Term(app_term, t0), t1(t1), t2(t2) { }
  App(const Location& l, Type* t0, Term* t1, Term* t2) 
    : Term(app_term, l, t0), t1(t1), t2(t2) { }

  Term* abs() const { return t1; }
  Term* arg() const { return t2; }

  Term* t1;
  Term* t2;
};

// A function call of the form 't(t1, ..., tn)' where 't' is a 
// function (not an abstraction) and each 'ti' is an argument.
struct Call : Term {
  Call(Type* t0, Term* t1, Term_seq* ts) 
    : Term(call_term, t0), t1(t1), t2(ts) { }
  Call(const Location& l, Type* t0, Term* t1, Term_seq* ts) 
    : Term(call_term, l, t0), t1(t1), t2(ts) { }

  Term* fn() const { return t1; }
  Term_seq* args() const { return t2; }

  Term* t1;
  Term_seq* t2;
};


// A definition of the form 'def n = t'.
//
// TODO: Refactor this so that the 'n=t' part is an init
// expression (see below);
struct Def : Term {
  Def(Type* t, Name* n, Expr* v)
    : Term(def_term, t), t1(n), t2(v) { }
  Def(const Location& l ,Type* t, Name* n, Expr* v)
    : Term(def_term, l, t), t1(n), t2(v) { }

  Name* name() const { return t1; }
  Expr* value() const { return t2; }

  Name* t1;
  Expr* t2;
};

// An initializer term of the form 'n = t' where 'n' is a name
// and 't' is the value that name takes on.
struct Init : Term {
  Init(Type* t, Name* n, Expr* v)
    : Term(init_term, t), t1(n), t2(v) { }
  Init(const Location& l ,Type* t, Name* n, Expr* v)
    : Term(init_term, l, t), t1(n), t2(v) { }

  Name* name() const { return t1; }
  Expr* value() const { return t2; }

  Name* t1;
  Expr* t2;
};

// A tuple of the form '{t1, ..., tn}' where each 'ti' is a term.
struct Tuple : Term {
  Tuple(Type* t, Term_seq* ts)
    : Term(tuple_term, t), t1(ts) { }
  Tuple(const Location& l, Type* t, Term_seq* ts)
    : Term(tuple_term, l, t), t1(ts) { }

  Term_seq* elems() const { return t1; }

  Term_seq* t1;
};

// A list of the form '[t1, ..., tn]' where each 'ti' is a term.
struct List : Term {
  List(Type* t, Term_seq* ts)
    : Term(list_term, t), t1(ts) { }
  List(const Location& l, Type* t, Term_seq* ts)
    : Term(list_term, l, t), t1(ts) { }

  Term_seq* elems() const { return t1; }

  Term_seq* t1;
};

// A record of the form '{n1=t1, ..., nn=tn}' where each ti is
// a initializer. Note that each subterm is an Init term.
//
// TODO: It might make sense to derive this from tupe. A
// record is a tuple of named elements.
struct Record : Term {
  Record(Type* t, Term_seq* ts)
    : Term(record_term, t), t1(ts) { }
  Record(const Location& l, Type* t, Term_seq* ts)
    : Term(record_term, l, t), t1(ts) { }

  Term_seq* members() const { return t1; }

  Term_seq* t1;
};

// A comma term of the form '(e1, ..., en)' is simply a sequence
// of expressions. These are used internally to represent
// function arguments or parameter types. 
struct Comma : Term {
  Comma(Type* t, Expr_seq* ts)
    : Term(comma_term, t), t1(ts) { }
  Comma(const Location& l, Type* t, Expr_seq* ts)
    : Term(comma_term, l, t), t1(ts) { }

  Expr_seq* elems() const { return t1; }

  Expr_seq* t1;
};

// A projection of an element in a tuple.
struct Proj : Term {
  Proj(Type* t, Term* t0, Term* n)
    : Term(proj_term, t), t1(t0), t2(n) { }
  Proj(const Location& l, Type* t, Term* t0, Term* n)
    : Term(proj_term, l, t), t1(t0), t2(n) { }

  Term* tuple() const { return t1; }
  Term* elem() const { return t2; }

  Term* t1;
  Term* t2;
};

// A projection of a field of a record.
struct Mem : Term {
  Mem(Type* t, Term* t0, Term* n)
    : Term(mem_term, t), t1(t0), t2(n) { }
  Mem(const Location& l, Type* t, Term* t0, Term* n)
    : Term(mem_term, l, t), t1(t0), t2(n) { }

  Term* record() const { return t1; }
  Term* member() const { return t2; }

  Term* t1;
  Term* t2;
};

// A column projection for a table
struct Col : Term {
  Col(Type* t, Term* t0, Term* n)
    : Term(col_term, t), t1(t0), t2(n) { }
  Col(const Location& l, Type* t, Term* t0, Term* n)
    : Term(col_term, l, t), t1(t0), t2(n) { }

  Term* table() const { return t1; }
  Term* attr() const { return t2; }

  Term* t1;
  Term* t2;
};

// Represefnts a reference to a declared entity in the program 
// (e.g., a variable, function, etc). Note that the type of the
// reference is the same as that of its referred-to expression.
struct Ref : Term {
  Ref(Expr* e)
    : Term(ref_term, e->tr), t1(e) { }
  Ref(const Location& l, Expr* e)
    : Term(ref_term, l, e->tr), t1(e) { }

  Expr* decl() const { return t1; }

  Expr* t1;
};

// Prints an expression to the terminal.
struct Print : Term {
  Print(Type* t, Expr* e)
    : Term(print_term, t), t1(e) { }
  Print(const Location& l, Type* t, Expr* e)
    : Term(print_term, l, t), t1(e) { }

  Expr* expr() const { return t1; }

  Expr* t1;
};

// A program is a sequence of terms called statements.
struct Prog : Term {
  Prog(Type* t, Term_seq* ts)
    : Term(prog_term, t), t1(ts) { }

  Term_seq* stmts() const { return t1; }

  Term_seq* t1;
};

// select t1 from t2 where t3
// t1 is a Comma term where each subterm is a Name
// t2 is a Table term
// t3 is anything that has type bool. Most commonly a term like and, or, equals, less, not
struct Select_from_where : Term {
  Select_from_where(Type* t, Term* t1, Term* t2, Term* t3)
    : Term(select_term, t), t1(t1), t2(t2), t3(t3) { }
  Select_from_where(const Location& l, Type* t, Term* t1, Term* t2, Term* t3)
    : Term(select_term, l, t), t1(t1), t2(t2), t3(t3) { }

  Term* projection_list() const { return t1; }
  Term* table() const { return t2; }
  Term* cond() const { return t3; }

  Term* t1;
  Term* t2;
  Term* t3;
};

// A term of form t1 join t2 on t3
// Evaluates to be a table
// t1 and t2 must have type table
// t3 must evaluate to bool
struct Join : Term {
  Join(Type* t, Term* t1, Term* t2, Term* t3)
    : Term(join_on_term, t), t1(t1), t2(t2), t3(t3) { }
  Join(const Location& l, Type* t, Term* t1, Term* t2, Term* t3)
    : Term(join_on_term, l, t), t1(t1), t2(t2), t3(t3){ }

  Term* table_a() const { return t1; }
  Term* table_b() const { return t2; }
  Term* join_cond() const { return t3; }

  Term* t1;
  Term* t2;
  Term* t3;
};

// t1 union t2
// t1 and t2 is either a set, tuple, or table
struct Union: Term {
  Union(Type* t, Term* t1, Term* t2)
    : Term(union_term, t), t1(t1), t2(t2) { }
  Union(const Location&l, Type* t, Term* t1, Term* t2)
    : Term(union_term, l, t), t1(t1), t2(t2) { }

  Term* t1;
  Term* t2;
};

// t1 intersect t2 
// t1 and t2 is either a set, tuple, or table
struct Intersect : Term {
  Intersect(Type* t, Term* t1, Term* t2)
    : Term(intersect_term, t), t1(t1), t2(t2) { }
  Intersect(const Location&l, Type* t, Term* t1, Term* t2)
    : Term(intersect_term, l, t), t1(t1), t2(t2) { }

  Term* t1;
  Term* t2;
};

// t1 except t2
// t1 and t2 either a set, tuple, or table
struct Except : Term {
  Except(Type* t, Term* t1, Term* t2)
    : Term(except_term, t), t1(t1), t2(t2) { }
  Except(const Location&l, Type* t, Term* t1, Term* t2)
    : Term(except_term, l, t), t1(t1), t2(t2) { }

  Term* t1;
  Term* t2;
};

// -------------------------------------------------------------------------- //
// Types

// Represents the type of a type.
struct Kind_type : Type {
  Kind_type()
    : Type(kind_type, nullptr) { }
  Kind_type(const Location& l)
    : Type(kind_type, l, nullptr) { }
};

// Represents the unit type.
struct Unit_type : Type {
  Unit_type(Type* k)
    : Type(unit_type, k) { }
  Unit_type(const Location& l, Type* k)
    : Type(unit_type, l, k) { }
};

// Represents the bool type.
struct Bool_type : Type {
  Bool_type(Type* k) 
    : Type(bool_type, k) { }
  Bool_type(const Location& l, Type* k) 
    : Type(bool_type, l, k) { }
};

// Represents the nat type.
struct Nat_type : Type {
  Nat_type(Type* k)
    : Type(nat_type, k) { }
  Nat_type(const Location& l, Type* k)
    : Type(nat_type, l, k) { }
};

// Represents the type of string vales.
struct Str_type : Type {
  Str_type(Type* k)
    : Type(str_type, k) { }
  Str_type(const Location& l, Type* k)
    : Type(str_type, l, k) { }
};

// An arrow type of the form 'T1->T2'.
struct Arrow_type : Type {
  Arrow_type(Type* k, Type* t1, Type* t2)
    : Type(arrow_type, k), t1(t1), t2(t2) { }
  Arrow_type(const Location& l, Type* k, Type* t1, Type* t2)
    : Type(arrow_type, l, k), t1(t1), t2(t2) { }

  Type* parm() const { return t1; }
  Type* result() const { return t2; }

  Type* t1;
  Type* t2;
};

// A function type of the form '(T1, ..., Tn) -> T'.
struct Fn_type : Type {
  Fn_type(Type* k, Type_seq* ts, Type* t)
    : Type(fn_type, k), t1(ts), t2(t) { }
  Fn_type(const Location& l, Type* k, Type_seq* ts, Type* t)
    : Type(fn_type, l, k), t1(ts), t2(t) { }

  Type_seq* parms() const { return t1; }
  Type* result() const { return t2; }

  Type_seq* t1;
  Type* t2;
};

// The type of a tuple has the form '{T1, ..., Tn}'.
struct Tuple_type : Type {
  Tuple_type(Type* k, Type_seq* ts)
    : Type(tuple_type, k), t1(ts) { }
  Tuple_type(const Location& l, Type* k, Type_seq* ts)
    : Type(tuple_type, l, k), t1(ts) { }

  Type_seq* types() const { return t1; }

  Type_seq* t1;
};

// The type of a list has the form [T].
struct List_type : Type {
  List_type(Type* k, Type* ts)
    : Type(list_type, k), t1(ts) { }
  List_type(const Location& l, Type* k, Type* ts)
    : Type(list_type, l, k), t1(ts) { }

  Type* type() const { return t1; }

  Type* t1;
};

// The type of a record has the form '{n1:T1, ..., nn:Tn}' 
// where each ni:Ti is a member variable.
//
// Note that each sub-term is a Var term.
struct Record_type : Type {
  Record_type(Type* k, Term_seq* ts)
    : Type(record_type, k), t1(ts) { }
  Record_type(const Location& l, Type* k, Term_seq* ts)
    : Type(record_type, l, k), t1(ts) { }

  Term_seq* members() const { return t1; }

  Term_seq* t1;
};

// A wildcard type of the form '*x:T' where 'x' is the name of the
// the wildcard and T is its type. Wildcard types are used to represent
// the type of a term when its complete type must be deduced from
// context.
struct Wild_type : Type {
  Wild_type(Type* k, Name* n, Type* t)
    : Type(wild_type, k), t1(n), t2(t) { }
  Wild_type(const Location& loc, Type* k, Name* n, Type* t)
    : Type(wild_type, loc, k), t1(n), t2(t) { }

  Name* name() const { return t1; }
  Type* type() const { return t2; }

  Name* t1;
  Type* t2;
  Type_seq* attr() const { return schema; }
  Type_seq* schema;
};


// -------------------------------------------------------------------------- //
// Properties

int size(Term*);

// Relations
bool is_same(Expr*, Expr*);
bool is_less(Expr*, Expr*);
bool is_step(Term*, Term*);
bool is_eval(Term*, Term*);

// An equivalence relation on expressions.
struct Expr_eq {
  bool operator()(Expr* e1, Expr* e2) const { return is_same(e1, e2); }
};

// A strict weak order on expressions.
struct Expr_less {
  bool operator()(Expr* e1, Expr* e2) const { return is_less(e1, e2); }
};


// -------------------------------------------------------------------------- //
// Printing

inline pretty_printer<Expr> 
pretty(Expr* e) { return pretty_printer<Expr>{e}; }

std::ostream& operator<<(std::ostream&, pretty_printer<Expr>);

#endif
