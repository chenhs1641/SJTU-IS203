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

//typedef std::map<Symbol, Symbol> ClassTable; // name, Class_ !!!copied
//ClassTable classTable;

typedef SymbolTable<Symbol, Symbol> ObjectEnvironment; // name, type
ObjectEnvironment objectEnv;

typedef SymbolTable<Symbol, Decl> ObjectEnvironment2; // name, function
ObjectEnvironment2 objectEnv2;

int flag = 0; // break/continue in loop?
bool flag0 = 0; // func have return?
// bool flag1 = 0; // have main func?

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
            if (!isValidCallName(curr_decl->getName())) semant_error(curr_decl)<<"cannot def printf!!!\n";
            if (objectEnv2.probe(curr_decl->getName()) != NULL) semant_error(curr_decl)<<"cannot redef func!!!\n";
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
            if (!isValidTypeName(curr_decl->getType())) semant_error(curr_decl)<<"cannot void var!!!\n";
            if (objectEnv.probe(curr_decl->getName()) != NULL) semant_error(curr_decl)<<"cannot redef var!!!\n";
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
        semant_error()<<"no main!!!\n";
        return;
    }
    Decl curr_decl = *(objectEnv2.probe(Main));
    Variables curr_paras = curr_decl->getVariables();
    Symbol return_type = curr_decl->getType();
    if (curr_paras->more(curr_paras->first())) semant_error(curr_decl)<<"main cannot have paras!!!\n";
    if (isValidTypeName(return_type)) semant_error(curr_decl)<<"main can only void type!!!\n";
}

void VariableDecl_class::check() {
    if (!isValidTypeName(variable->getType())) semant_error(variable)<<"cannot void this var!!!\n";
    if (objectEnv.probe(variable->getName()) != NULL) semant_error(variable)<<"cannot redef this var!!!\n";
    else objectEnv.addid(variable->getName(), new Symbol(variable->getType()));
}

void CallDecl_class::check() {
    objectEnv.enterscope();
    flag0 = 1;
    /*if (!strcmp(name->get_string(), "main"))
    {
        flag1 = 1;
        if (paras->more(paras->first())) semant_error(this)<<"main cannot have paras!!!\n";
        if (isValidTypeName(this->getType())) semant_error(this)<<"main can only void type!!!\n";
    }*/
    for (int i = paras->first(); paras->more(i); i = paras->next(i))
    {
        Variable curr_var = paras->nth(i);
        if (!isValidTypeName(curr_var->getType())) semant_error(curr_var)<<"cannot void xing var!!!\n";
        if (objectEnv.probe(curr_var->getName()) != NULL) semant_error(curr_var)<<"cannot redef xing var!!!\n";
        else
        {
            objectEnv.addid(curr_var->getName(), new Symbol(curr_var->getType()));
        }
    }
    this->getBody()->check(this->getType());
    if (flag0) semant_error(this)<<"no return!!!\n";
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
    if (strcmp(condition->checkType()->get_string(), "Bool")) semant_error(this)<<"if cannot unBool cond!!!\n";
    objectEnv.enterscope();
    thenexpr->check(type);
    objectEnv.exitscope();
    objectEnv.enterscope();
    elseexpr->check(type);
    objectEnv.exitscope();
}

void WhileStmt_class::check(Symbol type) {
    flag ++;
    if (strcmp(condition->checkType()->get_string(), "Bool")) semant_error(this)<<"while cannot unBool cond!!!\n";
    objectEnv.enterscope();
    body->check(type);
    objectEnv.exitscope();
    flag --;
}

void ForStmt_class::check(Symbol type) {
    flag ++;
    if (!initexpr->is_empty_Expr())
        if (!strcmp(initexpr->checkType()->get_string(), "String")) semant_error(this)<<"forinit cannot string cond!!!\n";
    if (!condition->is_empty_Expr())
        if (!strcmp(condition->checkType()->get_string(), "String")) semant_error(this)<<"forcond cannot string cond!!!\n";
    if (!loopact->is_empty_Expr())
        if (!strcmp(loopact->checkType()->get_string(), "String")) semant_error(this)<<"forloop cannot string cond!!!\n";
    objectEnv.enterscope();
    body->check(type);
    objectEnv.exitscope();
    flag --;
}

void ReturnStmt_class::check(Symbol type) {
    flag0 = 0;
    if (!sameType(value->checkType(), type)) semant_error(this)<<"return type error!!!\n";
}

void ContinueStmt_class::check(Symbol type) {
    if (flag == 0) semant_error(this)<<"not in loop cont!!!\n";
}

void BreakStmt_class::check(Symbol type) {
    if (flag == 0) semant_error(this)<<"not in loop break!!!\n";
}

Symbol Call_class::checkType(){
    if (!isValidCallName(name))
    {
        if (!actuals->more(actuals->first())) semant_error(this)<<"printf must have paras!!!\n";
        if (!strcmp(actuals->nth(actuals->first())->getType()->get_string(), "String"))
            semant_error(this)<<"printf first para must string type!!!\n";
        setType(Void);
        return type;
    }
    if (objectEnv2.probe(name) == NULL)
    {
        semant_error(this)<<"undef call!!!\n";
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
        Symbol curr_para_type = curr_paras->nth(i)->getType();
        if (!sameType(curr_expr_type, curr_para_type)) semant_error(this)<<"para type error!!!\n";
        //compare curr_expr_type with real function's curr_para_type
        i = actuals->next(i);
        j = curr_paras->next(j);
    }
    if (actuals->more(i) && !curr_paras->more(j)) semant_error(this)<<"too much call paras!!!\n";
    if (!actuals->more(i) && curr_paras->more(j)) semant_error(this)<<"too less call paras!!!\n";
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
        semant_error(this)<<"undef lvalue var!!!\n";
        setType(Void);
    }
    if (!sameType(value->checkType(), type)) semant_error(this)<<"assign type error!!!\n";
    return type;
}

Symbol Add_class::checkType(){
    Symbol ct1 = e1->checkType(), ct2 = e2->checkType();
    if (!strcmp(ct1->get_string(), "Int") && !strcmp(ct2->get_string(), "Int"))
    {
        setType(ct1);
        return type;
    }
    if (!strcmp(ct1->get_string(), "Float") && !strcmp(ct2->get_string(), "Float"))
    {
        setType(ct1);
        return type;
    }
    if (!strcmp(ct1->get_string(), "Int") && !strcmp(ct2->get_string(), "Float"))
    {
        setType(ct2);
        return type;
    }
    if (!strcmp(ct1->get_string(), "Float") && !strcmp(ct2->get_string(), "Int"))
    {
        setType(ct1);
        return type;
    }
    semant_error(this)<<"add type error!!!\n";
    setType(Void);
    return type;
}

Symbol Minus_class::checkType(){
    Symbol ct1 = e1->checkType(), ct2 = e2->checkType();
    if (!strcmp(ct1->get_string(), "Int") && !strcmp(ct2->get_string(), "Int"))
    {
        setType(ct1);
        return type;
    }
    if (!strcmp(ct1->get_string(), "Float") && !strcmp(ct2->get_string(), "Float"))
    {
        setType(ct1);
        return type;
    }
    if (!strcmp(ct1->get_string(), "Int") && !strcmp(ct2->get_string(), "Float"))
    {
        setType(ct2);
        return type;
    }
    if (!strcmp(ct1->get_string(), "Float") && !strcmp(ct2->get_string(), "Int"))
    {
        setType(ct1);
        return type;
    }
    semant_error(this)<<"min type error!!!\n";
    setType(Void);
    return type;
}

Symbol Multi_class::checkType(){
    Symbol ct1 = e1->checkType(), ct2 = e2->checkType();
    if (!strcmp(ct1->get_string(), "Int") && !strcmp(ct2->get_string(), "Int"))
    {
        setType(ct1);
        return type;
    }
    if (!strcmp(ct1->get_string(), "Float") && !strcmp(ct2->get_string(), "Float"))
    {
        setType(ct1);
        return type;
    }
    if (!strcmp(ct1->get_string(), "Int") && !strcmp(ct2->get_string(), "Float"))
    {
        setType(ct2);
        return type;
    }
    if (!strcmp(ct1->get_string(), "Float") && !strcmp(ct2->get_string(), "Int"))
    {
        setType(ct1);
        return type;
    }
    semant_error(this)<<"multi type error!!!\n";
    setType(Void);
    return type;
}

Symbol Divide_class::checkType(){
    Symbol ct1 = e1->checkType(), ct2 = e2->checkType();
    if (!strcmp(ct1->get_string(), "Int") && !strcmp(ct2->get_string(), "Int"))
    {
        setType(ct1);
        return type;
    }
    if (!strcmp(ct1->get_string(), "Float") && !strcmp(ct2->get_string(), "Float"))
    {
        setType(ct1);
        return type;
    }
    if (!strcmp(ct1->get_string(), "Int") && !strcmp(ct2->get_string(), "Float"))
    {
        setType(ct2);
        return type;
    }
    if (!strcmp(ct1->get_string(), "Float") && !strcmp(ct2->get_string(), "Int"))
    {
        setType(ct1);
        return type;
    }
    semant_error(this)<<"div type error!!!\n";
    setType(Void);
    return type;
}

Symbol Mod_class::checkType(){
    Symbol ct1 = e1->checkType(), ct2 = e2->checkType();
    if (!strcmp(ct1->get_string(), "Int") && !strcmp(ct2->get_string(), "Int"))
    {
        setType(ct1);
        return type;
    }
    semant_error(this)<<"mod type error!!!\n";
    setType(Int);
    return type;
}

Symbol Neg_class::checkType(){
    Symbol ct1 = e1->checkType();
    if (!strcmp(ct1->get_string(), "Int") || !strcmp(ct1->get_string(), "Float"))
    {
        setType(ct1);
        return type;
    }
    semant_error(this)<<"neg type error!!!\n";
    setType(Void);
    return type;
}

Symbol Lt_class::checkType(){
    Symbol ct1 = e1->checkType(), ct2 = e2->checkType();
    if (!strcmp(ct1->get_string(), "Int") && !strcmp(ct2->get_string(), "Int"))
    {
        setType(Bool);
        return type;
    }
    if (!strcmp(ct1->get_string(), "Float") && !strcmp(ct2->get_string(), "Float"))
    {
        setType(Bool);
        return type;
    }
    if (!strcmp(ct1->get_string(), "Int") && !strcmp(ct2->get_string(), "Float"))
    {
        setType(Bool);
        return type;
    }
    if (!strcmp(ct1->get_string(), "Float") && !strcmp(ct2->get_string(), "Int"))
    {
        setType(Bool);
        return type;
    }
    semant_error(this)<<"lt type error!!!\n";
    setType(Bool);
    return type;
}

Symbol Le_class::checkType(){
    Symbol ct1 = e1->checkType(), ct2 = e2->checkType();
    if (!strcmp(ct1->get_string(), "Int") && !strcmp(ct2->get_string(), "Int"))
    {
        setType(Bool);
        return type;
    }
    if (!strcmp(ct1->get_string(), "Float") && !strcmp(ct2->get_string(), "Float"))
    {
        setType(Bool);
        return type;
    }
    if (!strcmp(ct1->get_string(), "Int") && !strcmp(ct2->get_string(), "Float"))
    {
        setType(Bool);
        return type;
    }
    if (!strcmp(ct1->get_string(), "Float") && !strcmp(ct2->get_string(), "Int"))
    {
        setType(Bool);
        return type;
    }
    semant_error(this)<<"le type error!!!\n";
    setType(Bool);
    return type;
}

Symbol Equ_class::checkType(){
    Symbol ct1 = e1->checkType(), ct2 = e2->checkType();
    if (!strcmp(ct1->get_string(), "Int") && !strcmp(ct2->get_string(), "Int"))
    {
        setType(Bool);
        return type;
    }
    if (!strcmp(ct1->get_string(), "Float") && !strcmp(ct2->get_string(), "Float"))
    {
        setType(Bool);
        return type;
    }
    if (!strcmp(ct1->get_string(), "Int") && !strcmp(ct2->get_string(), "Float"))
    {
        setType(Bool);
        return type;
    }
    if (!strcmp(ct1->get_string(), "Float") && !strcmp(ct2->get_string(), "Int"))
    {
        setType(Bool);
        return type;
    }
    if (!strcmp(ct1->get_string(), "Bool") && !strcmp(ct2->get_string(), "Bool"))
    {
        setType(Bool);
        return type;
    }
    semant_error(this)<<"equ type error!!!\n";
    setType(Bool);
    return type;
}

Symbol Neq_class::checkType(){
    Symbol ct1 = e1->checkType(), ct2 = e2->checkType();
    if (!strcmp(ct1->get_string(), "Int") && !strcmp(ct2->get_string(), "Int"))
    {
        setType(Bool);
        return type;
    }
    if (!strcmp(ct1->get_string(), "Float") && !strcmp(ct2->get_string(), "Float"))
    {
        setType(Bool);
        return type;
    }
    if (!strcmp(ct1->get_string(), "Int") && !strcmp(ct2->get_string(), "Float"))
    {
        setType(Bool);
        return type;
    }
    if (!strcmp(ct1->get_string(), "Float") && !strcmp(ct2->get_string(), "Int"))
    {
        setType(Bool);
        return type;
    }
    if (!strcmp(ct1->get_string(), "Bool") && !strcmp(ct2->get_string(), "Bool"))
    {
        setType(Bool);
        return type;
    }
    semant_error(this)<<"neq type error!!!\n";
    setType(Bool);
    return type;
}

Symbol Ge_class::checkType(){
    Symbol ct1 = e1->checkType(), ct2 = e2->checkType();
    if (!strcmp(ct1->get_string(), "Int") && !strcmp(ct2->get_string(), "Int"))
    {
        setType(Bool);
        return type;
    }
    if (!strcmp(ct1->get_string(), "Float") && !strcmp(ct2->get_string(), "Float"))
    {
        setType(Bool);
        return type;
    }
    if (!strcmp(ct1->get_string(), "Int") && !strcmp(ct2->get_string(), "Float"))
    {
        setType(Bool);
        return type;
    }
    if (!strcmp(ct1->get_string(), "Float") && !strcmp(ct2->get_string(), "Int"))
    {
        setType(Bool);
        return type;
    }
    semant_error(this)<<"ge type error!!!\n";
    setType(Bool);
    return type;
}

Symbol Gt_class::checkType(){
    Symbol ct1 = e1->checkType(), ct2 = e2->checkType();
    if (!strcmp(ct1->get_string(), "Int") && !strcmp(ct2->get_string(), "Int"))
    {
        setType(Bool);
        return type;
    }
    if (!strcmp(ct1->get_string(), "Float") && !strcmp(ct2->get_string(), "Float"))
    {
        setType(Bool);
        return type;
    }
    if (!strcmp(ct1->get_string(), "Int") && !strcmp(ct2->get_string(), "Float"))
    {
        setType(Bool);
        return type;
    }
    if (!strcmp(ct1->get_string(), "Float") && !strcmp(ct2->get_string(), "Int"))
    {
        setType(Bool);
        return type;
    }
    semant_error(this)<<"gt type error!!!\n";
    setType(Bool);
    return type;
}

Symbol And_class::checkType(){
    Symbol ct1 = e1->checkType(), ct2 = e2->checkType();
    if (!strcmp(ct1->get_string(), "Bool") && !strcmp(ct2->get_string(), "Bool"))
    {
        setType(ct1);
        return type;
    }
    semant_error(this)<<"and type error!!!\n";
    setType(Bool);
    return type;
}

Symbol Or_class::checkType(){
    Symbol ct1 = e1->checkType(), ct2 = e2->checkType();
    if (!strcmp(ct1->get_string(), "Bool") && !strcmp(ct2->get_string(), "Bool"))
    {
        setType(ct1);
        return type;
    }
    semant_error(this)<<"or type error!!!\n";
    setType(Bool);
    return type;
}

Symbol Xor_class::checkType(){
    Symbol ct1 = e1->checkType(), ct2 = e2->checkType();
    if (!strcmp(ct1->get_string(), "Bool") && !strcmp(ct2->get_string(), "Bool"))
    {
        setType(ct1);
        return type;
    }
    if (!strcmp(ct1->get_string(), "Int") && !strcmp(ct2->get_string(), "Int"))
    {
        setType(ct1);
        return type;
    }
    semant_error(this)<<"xor type error!!!\n";
    setType(Void);
    return type;
}

Symbol Not_class::checkType(){
    Symbol ct1 = e1->checkType();
    if (!strcmp(ct1->get_string(), "Bool"))
    {
        setType(ct1);
        return type;
    }
    semant_error(this)<<"not type error!!!\n";
    setType(Bool);
    return type;
}

Symbol Bitand_class::checkType(){
    Symbol ct1 = e1->checkType(), ct2 = e2->checkType();
    if (!strcmp(ct1->get_string(), "Int") && !strcmp(ct2->get_string(), "Int"))
    {
        setType(ct1);
        return type;
    }
    semant_error(this)<<"bitand type error!!!\n";
    setType(Int);
    return type;
}

Symbol Bitor_class::checkType(){
    Symbol ct1 = e1->checkType(), ct2 = e2->checkType();
    if (!strcmp(ct1->get_string(), "Int") && !strcmp(ct2->get_string(), "Int"))
    {
        setType(ct1);
        return type;
    }
    semant_error(this)<<"bitor type error!!!\n";
    setType(Int);
    return type;
}

Symbol Bitnot_class::checkType(){
    Symbol ct1 = e1->checkType();
    if (!strcmp(ct1->get_string(), "Int"))
    {
        setType(ct1);
        return type;
    }
    semant_error(this)<<"bitnot type error!!!\n";
    setType(Int);
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
    semant_error(this)<<"undef var!!!\n";
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



