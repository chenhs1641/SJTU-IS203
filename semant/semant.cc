#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "semant.h"
#include "utilities.h"

extern int semant_debug;
extern char *curr_filename;

static ostream& error_stream = cerr;
static int semant_errors = 0;
static Decl curr_decl = 0;

typedef SymbolTable<Symbol, Symbol> ObjectEnvironment; // name, type
ObjectEnvironment objectEnv;

typedef SymbolTable<Symbol, Decl> ObjectEnvironment2; // name, function
ObjectEnvironment2 objectEnv2;

int flag = 0; // break/continue in loop?
bool flag0 = 0; // func have return?

///////////////////////////////////////////////
// helper func
///////////////////////////////////////////////


static ostream& semant_error() {
    semant_errors++;
    return error_stream;
}

static ostream& semant_error(tree_node *t) {
    error_stream << t->get_line_number() << ": ";
    return semant_error();
}

static ostream& internal_error(int lineno) {
    error_stream << "FATAL:" << lineno << ": ";
    return error_stream;
}

//////////////////////////////////////////////////////////////////////
//
// Symbols
//
// For convenience, a large number of symbols are predefined here.
// These symbols include the primitive type and method names, as well
// as fixed names used by the runtime system.
//
//////////////////////////////////////////////////////////////////////

static Symbol 
    Int,
    Float,
    String,
    Bool,
    Void,
    Main,
    print
    ;

bool isValidCallName(Symbol type) {
    return type != (Symbol)print;
}

bool isValidTypeName(Symbol type) {
    return type != Void;
}

//
// Initializing the predefined symbols.
//

static void initialize_constants(void) {
    // 4 basic types and Void type
    Bool        = idtable.add_string("Bool");
    Int         = idtable.add_string("Int");
    String      = idtable.add_string("String");
    Float       = idtable.add_string("Float");
    Void        = idtable.add_string("Void");  
    // Main function
    Main        = idtable.add_string("main");

    // classical function to print things, so defined here for call.
    print        = idtable.add_string("printf");
}

/*
    TODO :
    you should fill the following function defines, so that semant() can realize a semantic 
    analysis in a recursive way. 
    Of course, you can add any other functions to help.
*/

static bool sameType(Symbol name1, Symbol name2) {
    return strcmp(name1->get_string(), name2->get_string()) == 0;
}

static void install_calls(Decls decls) {
    for (int i = decls->first(); decls->more(i); i = decls->next(i))
    {
        Decl curr_decl = decls->nth(i);
        if (curr_decl->isCallDecl())
        {
            if (!isValidCallName(curr_decl->getName()))
            {
                semant_error(curr_decl)<<"Function printf cannot be redefination.\n";
                semant_error(curr_decl)<<"Function printf cannot have a name as printf.\n";
            }
            else if (objectEnv2.probe(curr_decl->getName()) != NULL)
                semant_error(curr_decl)<<"Function "<<curr_decl->getName()<<" was previously defined.\n";
            else objectEnv2.addid(curr_decl->getName(), new Decl(curr_decl));
        }
    }
}

static void install_globalVars(Decls decls) {
    for (int i = decls->first(); decls->more(i); i = decls->next(i))
    {
        Decl curr_decl = decls->nth(i);
        if (!curr_decl->isCallDecl())
        {
            if (objectEnv.probe(curr_decl->getName()) != NULL)
                semant_error(curr_decl)<<"var "<<curr_decl->getName()<<" was previously defined.\n";
            else if (!isValidTypeName(curr_decl->getType()))
                semant_error(curr_decl)<<"var "<<curr_decl->getName()
                <<" cannot be of type Void. Void can just be used as return type.\n";
            else objectEnv.addid(curr_decl->getName(), new Symbol(curr_decl->getType()));
        }
    }
}

static void check_calls(Decls decls) {
    for (int i = decls->first(); decls->more(i); i = decls->next(i))
    {
        Decl curr_decl = decls->nth(i);
        if (curr_decl->isCallDecl())
        {
            curr_decl->check();
        }
    }
}

static void check_main() {
    if (objectEnv2.probe(Main) == NULL)
    {
        semant_error()<<"Main function is not defined.\n";
        return;
    }
    Decl curr_decl = *(objectEnv2.probe(Main));
    Variables curr_paras = curr_decl->getVariables();
    Symbol return_type = curr_decl->getType();
    if (curr_paras->more(curr_paras->first())) semant_error(curr_decl)<<"Main function should not have any parameters.\n";
    if (isValidTypeName(return_type)) semant_error(curr_decl)<<"Main function should have return type Void.\n";
}

void VariableDecl_class::check() {
    if (objectEnv.probe(variable->getName()) != NULL)
        semant_error(variable)<<"var "<<variable->getName()<<" was previously defined.\n";
    else if (!isValidTypeName(variable->getType()))
        semant_error(variable)<<"var "<<variable->getName()
        <<" cannot be of type Void. Void can just be used as return type.\n";
    else objectEnv.addid(variable->getName(), new Symbol(variable->getType()));
}

void CallDecl_class::check() {
    objectEnv.enterscope();
    flag0 = 1;
    for (int i = paras->first(); paras->more(i); i = paras->next(i))
        if (i == 6) semant_error(this)<<"Function "<<this->getName()<<" has more than 6 parameters.\n";
    for (int i = paras->first(); paras->more(i); i = paras->next(i))
    {
        Variable curr_var = paras->nth(i);
        if (!isValidTypeName(curr_var->getType()))
            semant_error(this)<<"Function "<<this->getName()
            <<" 's parameter has an invalid type Void.\n";
        else if (objectEnv.probe(curr_var->getName()) != NULL)
            semant_error(this)<<"Function "<<this->getName()
            <<" 's parameter has a duplicate name "<<curr_var->getName()<<".\n";
        else
        {
            objectEnv.addid(curr_var->getName(), new Symbol(curr_var->getType()));
        }
    }
    this->getBody()->check(this->getType());
    if (flag0) semant_error(this)<<"Function "<<this->getName()<<" must have an overall return statement.\n";
    objectEnv.exitscope();
}

void StmtBlock_class::check(Symbol type) {
    for (int i = vars->first(); vars->more(i); i = vars->next(i))
        vars->nth(i)->check();
    for (int i = stmts->first(); stmts->more(i); i = stmts->next(i))
    {
        objectEnv.enterscope();
        stmts->nth(i)->check(type);
        objectEnv.exitscope();
    }
}

void IfStmt_class::check(Symbol type) {
    Symbol value_type = condition->checkType();
    if (!sameType(value_type, Bool))
        semant_error(this)<<"Condition must be a Bool, got "<<value_type<<"\n";
    objectEnv.enterscope();
    thenexpr->check(type);
    objectEnv.exitscope();
    objectEnv.enterscope();
    elseexpr->check(type);
    objectEnv.exitscope();
}

void WhileStmt_class::check(Symbol type) {
    flag ++;
    Symbol value_type = condition->checkType();
    if (!sameType(value_type, Bool))
        semant_error(this)<<"Condition must be a Bool, got "<<value_type<<"\n";
    objectEnv.enterscope();
    body->check(type);
    objectEnv.exitscope();
    flag --;
}

void ForStmt_class::check(Symbol type) {
    flag ++;
    if (!initexpr->is_empty_Expr())
        initexpr->checkType();
    if (!condition->is_empty_Expr())
    {
        Symbol value_type = condition->checkType();
        if (!sameType(value_type, Bool))
            semant_error(this)<<"Condition must be a Bool, got "<<value_type<<"\n";
    }
    if (!loopact->is_empty_Expr())
        loopact->checkType();
    objectEnv.enterscope();
    body->check(type);
    objectEnv.exitscope();
    flag --;
}

void ReturnStmt_class::check(Symbol type) {
    flag0 = 0;
    Symbol value_type = value->checkType();
    if (!sameType(value_type, type)) semant_error(this)<<"Returns "<<value_type<<" , but need "<<type<<"\n";
}

void ContinueStmt_class::check(Symbol type) {
    if (flag == 0) semant_error(this)<<"continue must be used in a loop sentence.\n";
}

void BreakStmt_class::check(Symbol type) {
    if (flag == 0) semant_error(this)<<"break must be used in a loop sentence.\n";
}

Symbol Call_class::checkType(){
    if (!isValidCallName(name))
    {
        if (!actuals->more(actuals->first()))
            semant_error(this)<<"printf() must has at last one parameter of type String.\n";
        else
        {
            for (int i = actuals->first(); actuals->more(i); i = actuals->next(i))
                actuals->nth(i)->checkType();
            if (!sameType(actuals->nth(actuals->first())->getType(), String))
                semant_error(this)<<"printf()'s first parameter must be of type String.\n";
        }
        setType(Void);
        return type;
    }
    if (objectEnv2.probe(name) == NULL)
    {
        semant_error(this)<<"Function "<<name<<" has not been defined.\n";
        setType(Void);
        return type;
    }
    Decl curr_decl = *(objectEnv2.probe(name));
    Variables curr_paras = curr_decl->getVariables();
    Symbol return_type = curr_decl->getType();
    int i = actuals->first();
    int j = curr_paras->first();
    while (actuals->more(i) && curr_paras->more(j))
    {
        Symbol curr_expr_type = actuals->nth(i)->checkType();
        Symbol curr_para_type = curr_paras->nth(j)->getType();
        if (!sameType(curr_expr_type, curr_para_type) && !sameType(curr_expr_type, Void))
        {
            semant_error(this)<<"Function "<<name<<", the "<<i + 1<<" parameter should be "
            <<curr_para_type<<" but provided a "<<curr_expr_type<<".\n";
            break;
        }
        i = actuals->next(i);
        j = curr_paras->next(j);
    }
    if (actuals->more(i) && !curr_paras->more(j))
        semant_error(this)<<"Function "<<name<<" called with wrong number of arguments.\n";
    if (!actuals->more(i) && curr_paras->more(j))
        semant_error(this)<<"Function "<<name<<" called with wrong number of arguments.\n";
    setType(return_type);
    return type;
}

Symbol Actual_class::checkType(){
    setType(expr->checkType());
    return type;
}

Symbol Assign_class::checkType(){
    if (objectEnv.probe(lvalue) != NULL)
    {
        setType(*(objectEnv.probe(lvalue)));
    }
    else if (objectEnv.lookup(lvalue) != NULL)
    {
        setType(*(objectEnv.lookup(lvalue)));
    }
    else
    {
        semant_error(this)<<"Left value "<<lvalue<<" has not been defined.\n";
        setType(Void);
    }
    Symbol value_type = value->checkType();
    if (!sameType(value_type, type))
        semant_error(this)<<"Right value must have type "<<type<<" , got "<<value_type<<".\n";
    return type;
}

Symbol Add_class::checkType(){
    Symbol ct1 = e1->checkType(), ct2 = e2->checkType();
    if (sameType(ct1, Int) && sameType(ct2, Int))
    {
        setType(Int);
        return type;
    }
    if (sameType(ct1, Float) && sameType(ct2, Float))
    {
        setType(Float);
        return type;
    }
    if (sameType(ct1, Int) && sameType(ct2, Float))
    {
        setType(Float);
        return type;
    }
    if (sameType(ct1, Float) && sameType(ct2, Int))
    {
        setType(Float);
        return type;
    }
    semant_error(this)<<"Cannot add a "<<ct1<<" and a "<<ct2<<".\n";
    setType(Void);
    return type;
}

Symbol Minus_class::checkType(){
    Symbol ct1 = e1->checkType(), ct2 = e2->checkType();
    if (sameType(ct1, Int) && sameType(ct2, Int))
    {
        setType(Int);
        return type;
    }
    if (sameType(ct1, Float) && sameType(ct2, Float))
    {
        setType(Float);
        return type;
    }
    if (sameType(ct1, Int) && sameType(ct2, Float))
    {
        setType(Float);
        return type;
    }
    if (sameType(ct1, Float) && sameType(ct2, Int))
    {
        setType(Float);
        return type;
    }
    semant_error(this)<<"Cannot minus a "<<ct1<<" by a "<<ct2<<".\n";
    setType(Void);
    return type;
}

Symbol Multi_class::checkType(){
    Symbol ct1 = e1->checkType(), ct2 = e2->checkType();
    if (sameType(ct1, Int) && sameType(ct2, Int))
    {
        setType(Int);
        return type;
    }
    if (sameType(ct1, Float) && sameType(ct2, Float))
    {
        setType(Float);
        return type;
    }
    if (sameType(ct1, Int) && sameType(ct2, Float))
    {
        setType(Float);
        return type;
    }
    if (sameType(ct1, Float) && sameType(ct2, Int))
    {
        setType(Float);
        return type;
    }
    semant_error(this)<<"Cannot multiply a "<<ct1<<" with a "<<ct2<<".\n";
    setType(Void);
    return type;
}

Symbol Divide_class::checkType(){
    Symbol ct1 = e1->checkType(), ct2 = e2->checkType();
    if (sameType(ct1, Int) && sameType(ct2, Int))
    {
        setType(Int);
        return type;
    }
    if (sameType(ct1, Float) && sameType(ct2, Float))
    {
        setType(Float);
        return type;
    }
    if (sameType(ct1, Int) && sameType(ct2, Float))
    {
        setType(Float);
        return type;
    }
    if (sameType(ct1, Float) && sameType(ct2, Int))
    {
        setType(Float);
        return type;
    }
    semant_error(this)<<"Cannot divide a "<<ct1<<" by a "<<ct2<<".\n";
    setType(Void);
    return type;
}

Symbol Mod_class::checkType(){
    Symbol ct1 = e1->checkType(), ct2 = e2->checkType();
    if (sameType(ct1, Int) && sameType(ct2, Int))
    {
        setType(Int);
        return type;
    }
    semant_error(this)<<"Cannot mod a "<<ct1<<" and a "<<ct2<<".\n";
    setType(Void);
    return type;
}

Symbol Neg_class::checkType(){
    Symbol ct1 = e1->checkType();
    if (sameType(ct1, Int) || sameType(ct1, Float))
    {
        setType(ct1);
        return type;
    }
    semant_error(this)<<"A "<<ct1<<" doesn't have a negative.\n";
    setType(Void);
    return type;
}

Symbol Lt_class::checkType(){
    Symbol ct1 = e1->checkType(), ct2 = e2->checkType();
    if (sameType(ct1, Int) && sameType(ct2, Int))
    {
        setType(Bool);
        return type;
    }
    if (sameType(ct1, Float) && sameType(ct2, Float))
    {
        setType(Bool);
        return type;
    }
    if (sameType(ct1, Int) && sameType(ct2, Float))
    {
        setType(Bool);
        return type;
    }
    if (sameType(ct1, Float) && sameType(ct2, Int))
    {
        setType(Bool);
        return type;
    }
    semant_error(this)<<"Cannot compare a "<<ct1<<" and a "<<ct2<<"(less).\n";
    setType(Void);
    return type;
}

Symbol Le_class::checkType(){
    Symbol ct1 = e1->checkType(), ct2 = e2->checkType();
    if (sameType(ct1, Int) && sameType(ct2, Int))
    {
        setType(Bool);
        return type;
    }
    if (sameType(ct1, Float) && sameType(ct2, Float))
    {
        setType(Bool);
        return type;
    }
    if (sameType(ct1, Int) && sameType(ct2, Float))
    {
        setType(Bool);
        return type;
    }
    if (sameType(ct1, Float) && sameType(ct2, Int))
    {
        setType(Bool);
        return type;
    }
    semant_error(this)<<"Cannot compare a "<<ct1<<" and a "<<ct2<<"(less than or equal).\n";
    setType(Void);
    return type;
}

Symbol Equ_class::checkType(){
    Symbol ct1 = e1->checkType(), ct2 = e2->checkType();
    if (sameType(ct1, Int) && sameType(ct2, Int))
    {
        setType(Bool);
        return type;
    }
    if (sameType(ct1, Float) && sameType(ct2, Float))
    {
        setType(Bool);
        return type;
    }
    if (sameType(ct1, Int) && sameType(ct2, Float))
    {
        setType(Bool);
        return type;
    }
    if (sameType(ct1, Float) && sameType(ct2, Int))
    {
        setType(Bool);
        return type;
    }
    if (sameType(ct1, Bool) && sameType(ct2, Bool))
    {
        setType(Bool);
        return type;
    }
    semant_error(this)<<"Cannot compare a "<<ct1<<" and a "<<ct2<<"(equal).\n";
    setType(Void);
    return type;
}

Symbol Neq_class::checkType(){
    Symbol ct1 = e1->checkType(), ct2 = e2->checkType();
    if (sameType(ct1, Int) && sameType(ct2, Int))
    {
        setType(Bool);
        return type;
    }
    if (sameType(ct1, Float) && sameType(ct2, Float))
    {
        setType(Bool);
        return type;
    }
    if (sameType(ct1, Int) && sameType(ct2, Float))
    {
        setType(Bool);
        return type;
    }
    if (sameType(ct1, Float) && sameType(ct2, Int))
    {
        setType(Bool);
        return type;
    }
    if (sameType(ct1, Bool) && sameType(ct2, Bool))
    {
        setType(Bool);
        return type;
    }
    semant_error(this)<<"Cannot compare a "<<ct1<<" and a "<<ct2<<"(not equal).\n";
    setType(Void);
    return type;
}

Symbol Ge_class::checkType(){
    Symbol ct1 = e1->checkType(), ct2 = e2->checkType();
    if (sameType(ct1, Int) && sameType(ct2, Int))
    {
        setType(Bool);
        return type;
    }
    if (sameType(ct1, Float) && sameType(ct2, Float))
    {
        setType(Bool);
        return type;
    }
    if (sameType(ct1, Int) && sameType(ct2, Float))
    {
        setType(Bool);
        return type;
    }
    if (sameType(ct1, Float) && sameType(ct2, Int))
    {
        setType(Bool);
        return type;
    }
    semant_error(this)<<"Cannot compare a "<<ct1<<" and a "<<ct2<<"(greater than or equal).\n";
    setType(Void);
    return type;
}

Symbol Gt_class::checkType(){
    Symbol ct1 = e1->checkType(), ct2 = e2->checkType();
    if (sameType(ct1, Int) && sameType(ct2, Int))
    {
        setType(Bool);
        return type;
    }
    if (sameType(ct1, Float) && sameType(ct2, Float))
    {
        setType(Bool);
        return type;
    }
    if (sameType(ct1, Int) && sameType(ct2, Float))
    {
        setType(Bool);
        return type;
    }
    if (sameType(ct1, Float) && sameType(ct2, Int))
    {
        setType(Bool);
        return type;
    }
    semant_error(this)<<"Cannot compare a "<<ct1<<" and a "<<ct2<<"(greater).\n";
    setType(Void);
    return type;
}

Symbol And_class::checkType(){
    Symbol ct1 = e1->checkType(), ct2 = e2->checkType();
    if (sameType(ct1, Bool) && sameType(ct2, Bool))
    {
        setType(Bool);
        return type;
    }
    semant_error(this)<<"Cannot use and(&&) between "<<ct1<<" and "<<ct2<<".\n";
    setType(Void);
    return type;
}

Symbol Or_class::checkType(){
    Symbol ct1 = e1->checkType(), ct2 = e2->checkType();
    if (sameType(ct1, Bool) && sameType(ct2, Bool))
    {
        setType(Bool);
        return type;
    }
    semant_error(this)<<"Cannot use or(||) between "<<ct1<<" and "<<ct2<<".\n";
    setType(Void);
    return type;
}

Symbol Xor_class::checkType(){
    Symbol ct1 = e1->checkType(), ct2 = e2->checkType();
    if (sameType(ct1, Bool) && sameType(ct2, Bool))
    {
        setType(Bool);
        return type;
    }
    if (sameType(ct1, Int) && sameType(ct2, Int))
    {
        setType(Int);
        return type;
    }
    semant_error(this)<<"Cannot use xor(^) between "<<ct1<<" and "<<ct2<<".\n";
    setType(Void);
    return type;
}

Symbol Not_class::checkType(){
    Symbol ct1 = e1->checkType();
    if (sameType(ct1, Bool))
    {
        setType(Bool);
        return type;
    }
    semant_error(this)<<"Cannot use not(!) upon "<<ct1<<".\n";
    setType(Void);
    return type;
}

Symbol Bitand_class::checkType(){
    Symbol ct1 = e1->checkType(), ct2 = e2->checkType();
    if (sameType(ct1, Int) && sameType(ct2, Int))
    {
        setType(Int);
        return type;
    }
    semant_error(this)<<"Cannot use bit-and(&) between "<<ct1<<" and "<<ct2<<".\n";
    setType(Void);
    return type;
}

Symbol Bitor_class::checkType(){
    Symbol ct1 = e1->checkType(), ct2 = e2->checkType();
    if (sameType(ct1, Int) && sameType(ct2, Int))
    {
        setType(Int);
        return type;
    }
    semant_error(this)<<"Cannot use bit-or(|) between "<<ct1<<" and "<<ct2<<".\n";
    setType(Void);
    return type;
}

Symbol Bitnot_class::checkType(){
    Symbol ct1 = e1->checkType();
    if (sameType(ct1, Int))
    {
        setType(Int);
        return type;
    }
    semant_error(this)<<"Cannot use unary not(~) upon "<<ct1<<".\n";
    setType(Void);
    return type;
}

Symbol Const_int_class::checkType(){
    setType(Int);
    return type;
}

Symbol Const_string_class::checkType(){
    setType(String);
    return type;
}

Symbol Const_float_class::checkType(){
    setType(Float);
    return type;
}

Symbol Const_bool_class::checkType(){
    setType(Bool);
    return type;
}

Symbol Object_class::checkType(){
    if (objectEnv.probe(var) != NULL)
    {
        setType(*(objectEnv.probe(var)));
        return type;
    }
    if (objectEnv.lookup(var) != NULL)
    {
        setType(*(objectEnv.lookup(var)));
        return type;
    }
    semant_error(this)<<"object "<<var<<" has not been defined.\n";
    setType(Void);
    return type;
}

Symbol No_expr_class::checkType(){
    setType(Void);
    return getType();
}

void Program_class::semant() {
    objectEnv.enterscope();
    objectEnv2.enterscope();
    initialize_constants();
    install_calls(decls);
    check_main();
    install_globalVars(decls);
    check_calls(decls);

    objectEnv.exitscope();
    objectEnv2.exitscope();
    if (semant_errors > 0) {
        cerr << "Compilation halted due to static semantic errors." << endl;
        exit(1);
    }
}



