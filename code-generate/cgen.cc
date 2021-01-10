
//**************************************************************
//
// Code generator SKELETON
//
//
//**************************************************************

#include "cgen.h"
#include "cgen_gc.h"

using namespace std;

extern void emit_string_constant(ostream& str, char *s);
extern int cgen_debug;

static char *CALL_REGS[] = {RDI, RSI, RDX, RCX, R8, R9};
static char *CALL_XMM[] = {XMM0, XMM1, XMM2, XMM3};

void cgen_helper(Decls decls, ostream& s);
void code(Decls decls, ostream& s);

int rsp = -56, pos = 0, bpos = 0, cpos = 0;
char *sym[6] = {new char, new char, new char, new char, new char, new char};
bool in_ex[6] = {0, 0, 0, 0, 0, 0};

typedef SymbolTable<Symbol, char> ObjectEnvironment; // name, type
ObjectEnvironment objectEnv00;
//////////////////////////////////////////////////////////////////
//
//
//    Helper Functions
//  
//
//////////////////////////////////////////////////////////////////

// you can add any helper functions here


//////////////////////////////////////////////////////////////////////
//
// Symbols
//
// For convenience, a large number of symbols are predefined here.
// These symbols include the primitive type and method names, as well
// as fixed names used by the runtime system.
//
//////////////////////////////////////////////////////////////////////
Symbol 
    Int,
    Float,
    String,
    Bool,
    Void,
    Main,
    print
    ;
//
// Initializing the predefined symbols.
//
static void initialize_constants(void)
{
    // 4 basic types and Void type
    Bool        = idtable.add_string("Bool");
    Int         = idtable.add_string("Int");
    String      = idtable.add_string("String");
    Float       = idtable.add_string("Float");
    Void        = idtable.add_string("Void");  
    // main function
    Main        = idtable.add_string("main");

    // classical function to print things, so defined here for call.
    print        = idtable.add_string("printf");
}


//*********************************************************
//
// Define method for code generation
//
//
//*********************************************************

void Program_class::cgen(ostream &os) 
{
  // spim wants comments to start with '#'
  os << "# start of generated code\n";

  initialize_constants();
  cgen_helper(decls,os);

  os << "\n# end of generated code\n";
}


//////////////////////////////////////////////////////////////////////////////
//
//  emit_* procedures
//
//  emit_X  writes code for operation "X" to the output stream.
//  There is an emit_X for each opcode X, as well as emit_ functions
//  for generating names according to the naming conventions (see emit.h)
//  and calls to support functions defined in the trap handler.
//
//  Register names and addresses are passed as strings.  See `emit.h'
//  for symbolic names you can use to refer to the strings.
//
//////////////////////////////////////////////////////////////////////////////

static void emit_mov(const char *source, const char *dest, ostream& s)
{
  s << MOV << source << COMMA << dest << endl;
}

static void emit_rmmov(const char *source_reg, int offset, const char *base_reg, ostream& s)
{
  s << MOV << source_reg << COMMA << offset << "(" << base_reg << ")"
      << endl;
}

static void emit_mrmov(const char *base_reg, int offset, const char *dest_reg, ostream& s)
{
  s << MOV << offset << "(" << base_reg << ")" << COMMA << dest_reg  
      << endl;
}

static void emit_irmov(const char *immidiate, const char *dest_reg, ostream& s)
{
  s << MOV << "$" << immidiate << COMMA << dest_reg  
      << endl;
}

static void emit_irmovl(const char *immidiate, const char *dest_reg, ostream& s)
{
  s << MOVL << "$" << immidiate << COMMA << dest_reg  
      << endl;
}

static void emit_immov(const char *immidiate, int offset, const char *base_reg, ostream& s)
{
  s << MOV << "$" << immidiate << COMMA << "(" << offset << ")" << base_reg  
      << endl;
}

static void emit_add(const char *source_reg, const char *dest_reg, ostream& s)
{
  s << ADD << source_reg << COMMA << dest_reg << endl;
}

static void emit_sub(const char *source_reg, const char *dest_reg, ostream& s)
{
  s << SUB << source_reg << COMMA << dest_reg << endl;
}

static void emit_mul(const char *source_reg, const char *dest_reg, ostream& s)
{
  s << MUL << source_reg << COMMA << dest_reg << endl;
}

static void emit_div(const char *dest_reg, ostream& s)
{
  s << DIV << dest_reg << endl;
}

static void emit_cqto(ostream &s)
{
  s << CQTO << endl;
}

static void emit_neg(const char *dest_reg, ostream& s)
{
  s << NEG << dest_reg << endl;
}

static void emit_and(const char *source_reg, const char *dest_reg, ostream& s)
{
  s << AND << source_reg << COMMA << dest_reg << endl;
}

static void emit_or(const char *source_reg, const char *dest_reg, ostream& s)
{
  s << OR << source_reg << COMMA << dest_reg << endl;
}

static void emit_xor(const char *source_reg, const char *dest_reg, ostream& s)
{
  s << XOR << source_reg << COMMA << dest_reg << endl;
}

static void emit_not(const char *dest_reg, ostream& s)
{
  s << NOT << " " << dest_reg << endl;
}

static void emit_movsd(const char *source, const char *dest, ostream& s)
{
  s << MOVSD << source << COMMA << dest << endl;
}

static void emit_movaps(const char *source, const char *dest, ostream& s)
{
  s << MOVAPS << source << COMMA << dest << endl;
}

static void emit_addsd(const char *source_reg, const char *dest_reg, ostream& s)
{
  s << ADDSD << source_reg << COMMA << dest_reg << endl;
}

static void emit_subsd(const char *source_reg, const char *dest_reg, ostream& s)
{
  s << SUBSD << source_reg << COMMA << dest_reg << endl;
}

static void emit_mulsd(const char *source_reg, const char *dest_reg, ostream& s)
{
  s << MULSD << source_reg << COMMA << dest_reg << endl;
}

static void emit_divsd(const char *source_reg, const char *dest_reg, ostream& s)
{
  s << DIVSD << source_reg << COMMA << dest_reg << endl;
}

static void emit_cmp(const char *source_reg, const char *dest_reg, ostream& s)
{
  s << CMP << source_reg << COMMA << dest_reg << endl;
}

static void emit_test(const char *source_reg, const char *dest_reg, ostream& s)
{
  s << TEST << source_reg << COMMA << dest_reg << endl;
}

static void emit_ucompisd(const char *source_reg, const char *dest_reg, ostream& s)
{
  s << UCOMPISD << source_reg << COMMA << dest_reg << endl;
}

static void emit_xorpd(const char *source_reg, const char *dest_reg, ostream& s)
{
  s << XORPD << source_reg << COMMA << dest_reg << endl;
}
static void emit_jmp(const char *dest, ostream& s)
{
  s << JMP << " " << dest << endl;
}

static void emit_jl(const char *dest, ostream& s)
{
  s << JL << " " << dest << endl;
}

static void emit_jle(const char *dest, ostream& s)
{
  s << JLE << " " << dest << endl;
}

static void emit_je(const char *dest, ostream& s)
{
  s << JE << " " << dest << endl;
}

static void emit_jne(const char *dest, ostream& s)
{
  s << JNE << " " << dest << endl;
}

static void emit_jg(const char *dest, ostream& s)
{
  s << JG << " " << dest << endl;
}

static void emit_jge(const char *dest, ostream& s)
{
  s << JGE << " " << dest << endl;
}

static void emit_jb(const char *dest, ostream& s)
{
  s << JB << " " << dest << endl;
}

static void emit_jbe(const char *dest, ostream& s)
{
  s << JBE << " " << dest << endl;
}

static void emit_ja(const char *dest, ostream& s)
{
  s << JA << " " << dest << endl;
}

static void emit_jae(const char *dest, ostream& s)
{
  s << JAE << " " << dest << endl;
}

static void emit_jp(const char *dest, ostream& s)
{
  s << JP << " " << dest << endl;
}

static void emit_jz(const char *dest, ostream& s)
{
  s << JZ << " " << dest << endl;
}

static void emit_jnz(const char *dest, ostream& s)
{
  s << JNZ << " " << dest << endl;
}

static void emit_call(const char *dest, ostream& s)
{
  s << CALL << " " << dest << endl;
}

static void emit_ret(ostream& s)
{
  s << RET << endl;
}

static void emit_push(const char *reg, ostream& s)
{
  s << PUSH << " " << reg << endl;
}

static void emit_pop(const char *reg, ostream& s)
{
  s << POP << " " << reg << endl;
}

static void emit_leave(ostream& s)
{
  s << LEAVE << endl;
}

static void emit_position(const char *p, ostream& s)
{
  s << p << ":" << endl;
}

static void emit_float_to_int(const char *float_mmx, const char *int_reg, ostream& s)
{
  s << CVTTSD2SIQ << float_mmx << COMMA << int_reg << endl;
}

static void emit_int_to_float(const char *int_reg, const char *float_mmx, ostream& s)
{
  s << CVTSI2SDQ << int_reg << COMMA << float_mmx << endl;
}
///////////////////////////////////////////////////////////////////////////////
//
// coding strings, ints, and booleans
//
// Seal has four kinds of constants: strings, ints, and booleans.
// This section defines code generation for each type.
//
// If you like, you can add any ***Entry::code_def() and ***Entry::code_ref()
// functions to help.
//
///////////////////////////////////////////////////////////////////////////////

//
// Strings
//
void StringEntry::code_ref(ostream& s)
{
  s << "$" << STRINGCONST_PREFIX << index;
}

//
// Emit code for a constant String.
//

void StringEntry::code_def(ostream& s)
{
  s << STRINGCONST_PREFIX << index << ":" << endl;
  s  << STRINGTAG ; emit_string_constant(s,str);                                                // align to word
}

//
// StrTable::code_string
// Generate a string object definition for every string constant in the 
// stringtable.
//
void StrTable::code_string_table(ostream& s)
{  
  for (List<StringEntry> *l = tbl; l; l = l->tl())
    l->hd()->code_def(s);
}

// the following 2 functions are useless, please DO NOT care about them
void FloatEntry::code_ref(ostream &s)
{
  s << FLOATTAG << index;
}

void IntEntry::code_def(ostream &s)
{
  s << GLOBAL;
}

//***************************************************
//
//  Emit global var and functions.
//
//***************************************************

static void emit_global_int(Symbol name, ostream& s) {
  s << GLOBAL << name << endl << 
  ALIGN << 8 << endl << 
  SYMBOL_TYPE << name << COMMA << OBJECT << endl <<
  SIZE << name << COMMA << 8 << endl << 
  name << ":" << endl << 
  INTTAG << 0 << endl;
}

static void emit_global_float(Symbol name, ostream& s) {
  s << GLOBAL << name << endl << 
  ALIGN << 8 << endl << 
  SYMBOL_TYPE << name << COMMA << OBJECT << endl <<
  SIZE << name << COMMA << 8 << endl << 
  name << ":" << endl <<
  FLOATTAG << 0 << endl <<
  FLOATTAG << 0 << endl;
}

static void emit_global_bool(Symbol name, ostream& s) {
  s << GLOBAL << name << endl << 
  ALIGN << 8 << endl << 
  SYMBOL_TYPE << name << COMMA << OBJECT << endl <<
  SIZE << name << COMMA << 8 << endl << 
  name << ":" << endl << 
  BOOLTAG << 0 << endl;
}

void code_global_data(Decls decls, ostream &str)
{
  bool have_data = 0;
  for (int i = decls->first(); decls->more(i); i = decls->next(i))
  {
    if (!decls->nth(i)->isCallDecl())
    {
      if (!have_data)
      {
        str << DATA << endl;
        have_data = 1;
      }
      if (decls->nth(i)->getType() == String) continue;
      Symbol name = decls->nth(i)->getName();
      if (decls->nth(i)->getType() == Int) emit_global_int(name, str);
      if (decls->nth(i)->getType() == Float) emit_global_float(name, str);
      if (decls->nth(i)->getType() == Bool) emit_global_bool(name, str);
    }
  }
}

void code_calls(Decls decls, ostream &str) {
  str << SECTION << RODATA << endl;
  // find strings for label LCxs;
  stringtable.code_string_table(str);
  str << TEXT << endl;
  for (int i = decls->first(); decls->more(i); i = decls->next(i))
  {
    if (decls->nth(i)->isCallDecl())
    {
      decls->nth(i)->code(str);
    }
  }
}

//***************************************************
//
//  Emit code to start the .text segment and to
//  declare the global names.
//
//***************************************************



//********************************************************
//
// Cgen helper helps to initialize and call code() function.
// You can do any initializing operations here
//
//********************************************************

void cgen_helper(Decls decls, ostream& s)
{

  code(decls, s);
}


void code(Decls decls, ostream& s)
{
  if (cgen_debug) cout << "Coding global data" << endl;
  code_global_data(decls, s);
  if (cgen_debug) cout << "Coding calls" << endl;
  code_calls(decls, s);
}

//******************************************************************
//
//   Fill in the following methods to produce code for the
//   appropriate expression.  You may add or remove parameters
//   as you wish, but if you do, remember to change the parameters
//   of the declarations in `seal-decl.h', `seal-expr.h' and `seal-stmt.h'
//   Sample code for constant integers, strings, and booleans are provided.
//   
//*****************************************************************

void CallDecl_class::code(ostream &s) {
  Symbol name = this->getName();
  s << GLOBAL << name << endl;
  s << SYMBOL_TYPE << name << COMMA << FUNCTION << endl;
  s << name << ":" << endl;
  emit_push(RBP, s);
  emit_mov(RSP, RBP, s);
  emit_push(RBX, s);
  emit_push(R10, s);
  emit_push(R11, s);
  emit_push(R12, s);
  emit_push(R13, s);
  emit_push(R14, s);
  emit_push(R15, s);
  objectEnv00.enterscope();
  int regs = 0, xmm = 0;
  for (int i = paras->first(); paras->more(i); i = paras->next(i))
  {
    emit_sub("$8", RSP, s);
    rsp -= 8;
    char* str = new char;
    sprintf(str, "%d(%%rbp)", rsp);
    objectEnv00.addid(paras->nth(i)->getName(), str);
    if (paras->nth(i)->getType() == Int || paras->nth(i)->getType() == Bool)
    {
      emit_rmmov(CALL_REGS[regs], rsp, RBP, s);
      regs ++;
    }
    else if (paras->nth(i)->getType() == Float)
    {
      emit_mov(CALL_XMM[xmm], str, s);
      xmm ++;
    }
  }
  this->getBody()->code(s);
  objectEnv00.exitscope();
  s << SIZE << name << COMMA << ".-" << name << endl;
  rsp = 0;
}

void StmtBlock_class::code(ostream &s){
  for (int i = vars->first(); vars->more(i); i = vars->next(i))
  {
    emit_sub("$8", RSP, s);
    rsp -= 8;
    char* str = new char;
    sprintf(str, "%d(%%rbp)", rsp);
    objectEnv00.addid(vars->nth(i)->getName(), str);
  }
  int thisbpos = bpos, thiscpos = cpos;
  for (int i = stmts->first(); stmts->more(i); i = stmts->next(i))
  {
    stmts->nth(i)->code(s);
    bpos = thisbpos;
    cpos = thiscpos;
  }
}

void IfStmt_class::code(ostream &s) {
  int thispos = pos;
  pos += 2;
  in_ex[0] = 1;
  condition->code(s);
  in_ex[0] = 0;
  emit_mov(sym[0], RAX, s);
  emit_test(RAX, RAX, s);
  char* str = new char;
  sprintf(str, ".POS%d", thispos);
  emit_jz(str, s);
  objectEnv00.enterscope();
  thenexpr->code(s);
  objectEnv00.exitscope();
  sprintf(str, ".POS%d", thispos + 1);
  emit_jmp(str, s);
  sprintf(str, ".POS%d", thispos);
  emit_position(str, s);
  objectEnv00.enterscope();
  elseexpr->code(s);
  objectEnv00.exitscope();
  sprintf(str, ".POS%d", thispos + 1);
  emit_position(str, s);
}

void WhileStmt_class::code(ostream &s) {
  int thispos = pos;
  bpos = pos + 1;
  cpos = pos;
  pos += 2;
  char* str = new char;
  sprintf(str, ".POS%d", thispos);
  emit_position(str, s);
  in_ex[0] = 1;
  condition->code(s);
  in_ex[0] = 0;
  emit_mov(sym[0], RAX, s);
  emit_test(RAX, RAX, s);
  sprintf(str, ".POS%d", thispos + 1);
  emit_jz(str, s);
  objectEnv00.enterscope();
  body->code(s);
  objectEnv00.exitscope();
  sprintf(str, ".POS%d", thispos);
  emit_jmp(str, s);
  sprintf(str, ".POS%d", thispos + 1);
  emit_position(str, s);
}

void ForStmt_class::code(ostream &s) {
  int thispos = pos;
  bpos = pos + 2;
  cpos = pos + 1;
  pos += 3;
  initexpr->code(s);
  char* str = new char;
  sprintf(str, ".POS%d", thispos);
  emit_position(str, s);
  in_ex[0] = 1;
  condition->code(s);
  in_ex[0] = 0;
  emit_mov(sym[0], RAX, s);
  emit_test(RAX, RAX, s);
  sprintf(str, ".POS%d", thispos + 2);
  emit_jz(str, s);
  objectEnv00.enterscope();
  body->code(s);
  objectEnv00.exitscope();
  sprintf(str, ".POS%d", thispos + 1);
  emit_position(str, s);
  loopact->code(s);
  sprintf(str, ".POS%d", thispos);
  emit_jmp(str, s);
  sprintf(str, ".POS%d", thispos + 2);
  emit_position(str, s);
}

void ReturnStmt_class::code(ostream &s) {
  if (!value->is_empty_Expr())
  {
    in_ex[0] = 1;
    value->code(s);
    in_ex[0] = 0;
    if (value->getType() == Float)
      emit_movaps(sym[0], XMM0, s);
    else
      emit_mov(sym[0], RAX, s);
  }
  emit_pop(R15, s);
  emit_pop(R14, s);
  emit_pop(R13, s);
  emit_pop(R12, s);
  emit_pop(R11, s);
  emit_pop(R10, s);
  emit_pop(RBX, s);
  emit_leave(s);
  emit_ret(s);
}

void ContinueStmt_class::code(ostream &s) {
  char* str = new char;
  sprintf(str, ".POS%d", cpos);
  emit_jmp(str, s);
}

void BreakStmt_class::code(ostream &s) {
  char* str = new char;
  sprintf(str, ".POS%d", bpos);
  emit_jmp(str, s);
}

void Call_class::code(ostream &s) {
  if (this->getName() == print)
  {
    // please set %eax to the number of Float parameters, num.
    // 把%eax赋值为Float类型的参数个数, num
    int n = 0;
    char *num = new char;
    for (int i = actuals->first(); actuals->more(i); i = actuals->next(i))
    {
      if (actuals->nth(i)->getType() == Float) n ++;
      in_ex[i] = 1;
      actuals->nth(i)->code(s);
      in_ex[i] = 0;
    }
    int regs = 0, xmm = 0;
    for (int i = actuals->first(); actuals->more(i); i = actuals->next(i))
      if (actuals->nth(i)->getType() == Int || actuals->nth(i)->getType() == Bool || actuals->nth(i)->getType() == String)
      {
        emit_mov(sym[i], CALL_REGS[regs], s);
        regs ++;
      }
      else if (actuals->nth(i)->getType() == Float)
      {
        emit_movsd(sym[i], CALL_XMM[xmm], s);
        xmm ++;
      }
    emit_sub("$8", RSP, s);
    rsp -= 8;
    sprintf(num, "%d", n);
    emit_irmovl(num, EAX, s);
    emit_call("printf", s);
    return;
  }
  else
  {
    bool tmp_inex[6];
    char* tmp_sym[6] = {new char, new char, new char, new char, new char, new char};
    for (int i = 0; i < 6; i ++)
    {
      tmp_inex[i] = in_ex[i];
      in_ex[i] = 0;
      strcpy(tmp_sym[i], sym[i]);
    }
    for (int i = actuals->first(); actuals->more(i); i = actuals->next(i))
    {
      in_ex[i] = 1;
      actuals->nth(i)->code(s);
      in_ex[i] = 0;
    }
    int regs = 0, xmm = 0;
    for (int i = actuals->first(); actuals->more(i); i = actuals->next(i))
      if (actuals->nth(i)->getType() == Int || actuals->nth(i)->getType() == Bool)
      {
        emit_mov(sym[i], CALL_REGS[regs], s);
        regs ++;
      }
      else if (actuals->nth(i)->getType() == Float)
      {
        emit_movsd(sym[i], CALL_XMM[xmm], s);
        xmm ++;
      }
    emit_call(this->getName()->get_string(), s);
    if (this->getType() == Int || this->getType() == Bool || this->getType() == Float)
    {
      emit_sub("$8", RSP, s);
      rsp -= 8;
      if (this->getType() == Float)
      {
        char* s0 = new char;
        sprintf(s0, "%d(%%rbp)", rsp);
        emit_movsd(XMM0, s0, s);
      }
      else
        emit_rmmov(RAX, rsp, RBP, s);
      for (int i = 0; i < 6; i ++)
      {
        in_ex[i] = tmp_inex[i];
        strcpy(sym[i], tmp_sym[i]);
      }
      char* str = new char;
      sprintf(str, "%d(%%rbp)", rsp);
      for (int i = 0;i < 6;i ++)
        if (in_ex[i])
          strcpy(sym[i], str);
    }
  }
}

void Actual_class::code(ostream &s) {
  if (!this->is_empty_Expr()) expr->code(s);
}

void Assign_class::code(ostream &s) {
  bool tmp_inex[6];
  char* tmp_sym[6] = {new char, new char, new char, new char, new char, new char};
  for (int i = 0; i < 6; i ++)
  {
    tmp_inex[i] = in_ex[i];
    in_ex[i] = 0;
    strcpy(tmp_sym[i], sym[i]);
  }
  in_ex[0] = 1;
  value->code(s);
  in_ex[0] = 0;
  emit_mov(sym[0], RAX, s);
  char* str = new char;
  if (objectEnv00.probe(lvalue) != NULL)
    strcpy(str, objectEnv00.probe(lvalue));
  else if (objectEnv00.lookup(lvalue) != NULL)
    strcpy(str, objectEnv00.lookup(lvalue));
  else
  {
    strcpy(str, lvalue->get_string());
    strcat(str, "(");
    strcat(str, RIP);
    strcat(str, ")");
  }
  emit_mov(RAX, str, s);
  for (int i = 0; i < 6; i ++)
  {
    in_ex[i] = tmp_inex[i];
    strcpy(sym[i], tmp_sym[i]);
  }
  for (int i = 0;i < 6;i ++)
    if (in_ex[i])
      strcpy(sym[i], str);
}

void Add_class::code(ostream &s) {
  bool tmp_inex[6];
  char* tmp_sym[6] = {new char, new char, new char, new char, new char, new char};
  for (int i = 0; i < 6; i ++)
  {
    tmp_inex[i] = in_ex[i];
    in_ex[i] = 0;
    strcpy(tmp_sym[i], sym[i]);
  }
  in_ex[0] = 1;
  e1->code(s);
  in_ex[0] = 0;
  in_ex[1] = 1;
  e2->code(s);
  in_ex[1] = 0;
  emit_sub("$8", RSP, s);
    rsp -= 8;
  if (e1->getType() == Int && e2->getType() == Int)
  {
    emit_mov(sym[0], RBX, s);
    emit_mov(sym[1], R10, s);
    emit_add(RBX, R10, s);
    emit_rmmov(R10, rsp, RBP, s);
  }
  else
  {
    if (e1->getType() == Int)
    {
      emit_mov(sym[0], RBX, s);
      emit_int_to_float(RBX, XMM4, s);
    }
    else
      emit_movsd(sym[0], XMM4, s);
    if (e2->getType() == Int)
    {
      emit_mov(sym[1], RBX, s);
      emit_int_to_float(RBX, XMM5, s);
    }
    else
      emit_movsd(sym[1], XMM5, s);
    emit_addsd(XMM4, XMM5, s);
    char* s0 = new char;
    sprintf(s0, "%d(%%rbp)", rsp);
    emit_movsd(XMM5, s0, s);
  }
  for (int i = 0; i < 6; i ++)
  {
    in_ex[i] = tmp_inex[i];
    strcpy(sym[i], tmp_sym[i]);
  }
  char* str = new char;
  sprintf(str, "%d(%%rbp)", rsp);
  for (int i = 0;i < 6;i ++)
    if (in_ex[i])
      strcpy(sym[i], str);
}

void Minus_class::code(ostream &s) {
  bool tmp_inex[6];
  char* tmp_sym[6] = {new char, new char, new char, new char, new char, new char};
  for (int i = 0; i < 6; i ++)
  {
    tmp_inex[i] = in_ex[i];
    in_ex[i] = 0;
    strcpy(tmp_sym[i], sym[i]);
  }
  in_ex[0] = 1;
  e1->code(s);
  in_ex[0] = 0;
  in_ex[1] = 1;
  e2->code(s);
  in_ex[1] = 0;
  emit_sub("$8", RSP, s);
  rsp -= 8;
  if (e1->getType() == Int && e2->getType() == Int)
  {
    emit_mov(sym[0], RBX, s);
    emit_mov(sym[1], R10, s);
    emit_sub(R10, RBX, s);
    emit_rmmov(RBX, rsp, RBP, s);
  }
  else
  {
    if (e1->getType() == Int)
    {
      emit_mov(sym[0], RBX, s);
      emit_int_to_float(RBX, XMM4, s);
    }
    else
      emit_movsd(sym[0], XMM4, s);
    if (e2->getType() == Int)
    {
      emit_mov(sym[1], RBX, s);
      emit_int_to_float(RBX, XMM5, s);
    }
    else
      emit_movsd(sym[1], XMM5, s);
    emit_subsd(XMM5, XMM4, s);
    char* s0 = new char;
    sprintf(s0, "%d(%%rbp)", rsp);
    emit_movsd(XMM4, s0, s);
  }
  for (int i = 0; i < 6; i ++)
  {
    in_ex[i] = tmp_inex[i];
    strcpy(sym[i], tmp_sym[i]);
  }
  char* str = new char;
  sprintf(str, "%d(%%rbp)", rsp);
  for (int i = 0;i < 6;i ++)
    if (in_ex[i])
      strcpy(sym[i], str);
}

void Multi_class::code(ostream &s) {
  bool tmp_inex[6];
  char* tmp_sym[6] = {new char, new char, new char, new char, new char, new char};
  for (int i = 0; i < 6; i ++)
  {
    tmp_inex[i] = in_ex[i];
    in_ex[i] = 0;
    strcpy(tmp_sym[i], sym[i]);
  }
  in_ex[0] = 1;
  e1->code(s);
  in_ex[0] = 0;
  in_ex[1] = 1;
  e2->code(s);
  in_ex[1] = 0;
  emit_sub("$8", RSP, s);
  rsp -= 8;
  if (e1->getType() == Int && e2->getType() == Int)
  {
    emit_mov(sym[0], RBX, s);
    emit_mov(sym[1], R10, s);
    emit_mul(R10, RBX, s);
    emit_rmmov(RBX, rsp, RBP, s);
  }
  else
  {
    if (e1->getType() == Int)
    {
      emit_mov(sym[0], RBX, s);
      emit_int_to_float(RBX, XMM4, s);
    }
    else
      emit_movsd(sym[0], XMM4, s);
    if (e2->getType() == Int)
    {
      emit_mov(sym[1], RBX, s);
      emit_int_to_float(RBX, XMM5, s);
    }
    else
      emit_movsd(sym[1], XMM5, s);
    emit_mulsd(XMM5, XMM4, s);
    char* s0 = new char;
    sprintf(s0, "%d(%%rbp)", rsp);
    emit_movsd(XMM4, s0, s);
  }
  for (int i = 0; i < 6; i ++)
  {
    in_ex[i] = tmp_inex[i];
    strcpy(sym[i], tmp_sym[i]);
  }
  char* str = new char;
  sprintf(str, "%d(%%rbp)", rsp);
  for (int i = 0;i < 6;i ++)
    if (in_ex[i])
      strcpy(sym[i], str);
}

void Divide_class::code(ostream &s) {
  bool tmp_inex[6];
  char* tmp_sym[6] = {new char, new char, new char, new char, new char, new char};
  for (int i = 0; i < 6; i ++)
  {
    tmp_inex[i] = in_ex[i];
    in_ex[i] = 0;
    strcpy(tmp_sym[i], sym[i]);
  }
  in_ex[0] = 1;
  e1->code(s);
  in_ex[0] = 0;
  in_ex[1] = 1;
  e2->code(s);
  in_ex[1] = 0;
  emit_sub("$8", RSP, s);
  rsp -= 8;
  if (e1->getType() == Int && e2->getType() == Int)
  {
    emit_mov(sym[0], RAX, s);
    emit_cqto(s);
    emit_mov(sym[1], RBX, s);
    s << "\tidivq\t" << RBX << "\n";
    emit_rmmov(RAX, rsp, RBP, s);
  }
  else
  {
    if (e1->getType() == Int)
    {
      emit_mov(sym[0], RBX, s);
      emit_int_to_float(RBX, XMM4, s);
    }
    else
      emit_movsd(sym[0], XMM4, s);
    if (e2->getType() == Int)
    {
      emit_mov(sym[1], RBX, s);
      emit_int_to_float(RBX, XMM5, s);
    }
    else
      emit_movsd(sym[1], XMM5, s);
    emit_divsd(XMM5, XMM4, s);
    char* s0 = new char;
    sprintf(s0, "%d(%%rbp)", rsp);
    emit_movsd(XMM4, s0, s);
  }
  for (int i = 0; i < 6; i ++)
  {
    in_ex[i] = tmp_inex[i];
    strcpy(sym[i], tmp_sym[i]);
  }
  char* str = new char;
  sprintf(str, "%d(%%rbp)", rsp);
  for (int i = 0;i < 6;i ++)
    if (in_ex[i])
      strcpy(sym[i], str);
}

void Mod_class::code(ostream &s) {
  bool tmp_inex[6];
  char* tmp_sym[6] = {new char, new char, new char, new char, new char, new char};
  for (int i = 0; i < 6; i ++)
  {
    tmp_inex[i] = in_ex[i];
    in_ex[i] = 0;
    strcpy(tmp_sym[i], sym[i]);
  }
  in_ex[0] = 1;
  e1->code(s);
  in_ex[0] = 0;
  in_ex[1] = 1;
  e2->code(s);
  in_ex[1] = 0;
  emit_sub("$8", RSP, s);
  rsp -= 8;
  emit_mov(sym[0], RAX, s);
  emit_cqto(s);
  emit_mov(sym[1], RBX, s);
  s << "\tidivq\t" << RBX << "\n";
  emit_rmmov(RDX, rsp, RBP, s);
  for (int i = 0; i < 6; i ++)
  {
    in_ex[i] = tmp_inex[i];
    strcpy(sym[i], tmp_sym[i]);
  }
  char* str = new char;
  sprintf(str, "%d(%%rbp)", rsp);
  for (int i = 0;i < 6;i ++)
    if (in_ex[i])
      strcpy(sym[i], str);
}

void Neg_class::code(ostream &s) {
  bool tmp_inex[6];
  char* tmp_sym[6] = {new char, new char, new char, new char, new char, new char};
  for (int i = 0; i < 6; i ++)
  {
    tmp_inex[i] = in_ex[i];
    in_ex[i] = 0;
    strcpy(tmp_sym[i], sym[i]);
  }
  in_ex[0] = 1;
  e1->code(s);
  in_ex[0] = 0;
  emit_sub("$8", RSP, s);
  rsp -= 8;
  emit_mov(sym[0], RAX, s);
  emit_neg(RAX, s);
  emit_rmmov(RAX, rsp, RBP, s);
  for (int i = 0; i < 6; i ++)
  {
    in_ex[i] = tmp_inex[i];
    strcpy(sym[i], tmp_sym[i]);
  }
  char* str = new char;
  sprintf(str, "%d(%%rbp)", rsp);
  for (int i = 0;i < 6;i ++)
    if (in_ex[i])
      strcpy(sym[i], str);
}

void Lt_class::code(ostream &s) {
  bool tmp_inex[6];
  char* tmp_sym[6] = {new char, new char, new char, new char, new char, new char};
  for (int i = 0; i < 6; i ++)
  {
    tmp_inex[i] = in_ex[i];
    in_ex[i] = 0;
    strcpy(tmp_sym[i], sym[i]);
  }
  int thispos = pos;
  pos += 2;
  in_ex[0] = 1;
  e1->code(s);
  in_ex[0] = 0;
  in_ex[1] = 1;
  e2->code(s);
  in_ex[1] = 0;
  emit_sub("$8", RSP, s);
  rsp -= 8;
  if (e1->getType() == Int && e2->getType() == Int)
  {
    emit_mov(sym[0], RAX, s);
    emit_mov(sym[1], RDX, s);
    emit_cmp(RDX, RAX, s);
  }
  else
  {
    if (e1->getType() == Int)
    {
      emit_mov(sym[0], RAX, s);
      emit_int_to_float(RAX, XMM1, s);
    }
    else
      emit_mov(sym[0], XMM1, s);
    if (e2->getType() == Int)
    {
      emit_mov(sym[1], RAX, s);
      emit_int_to_float(RAX, XMM0, s);
    }
    else
      emit_movsd(sym[1], XMM0, s);
    emit_ucompisd(XMM0, XMM1, s);
  }
  char* str = new char;
  sprintf(str, ".POS%d", thispos);
  emit_jl(str, s);
  emit_mov("$0", RAX, s);
  sprintf(str, ".POS%d", thispos + 1);
  emit_jmp(str, s);
  sprintf(str, ".POS%d", thispos);
  emit_position(str, s);
  emit_mov("$1", RAX, s);
  sprintf(str, ".POS%d", thispos + 1);
  emit_position(str, s);
  emit_rmmov(RAX, rsp, RBP, s);
  for (int i = 0; i < 6; i ++)
  {
    in_ex[i] = tmp_inex[i];
    strcpy(sym[i], tmp_sym[i]);
  }
  sprintf(str, "%d(%%rbp)", rsp);
  for (int i = 0;i < 6;i ++)
    if (in_ex[i])
      strcpy(sym[i], str);
}

void Le_class::code(ostream &s) {
  bool tmp_inex[6];
  char* tmp_sym[6] = {new char, new char, new char, new char, new char, new char};
  for (int i = 0; i < 6; i ++)
  {
    tmp_inex[i] = in_ex[i];
    in_ex[i] = 0;
    strcpy(tmp_sym[i], sym[i]);
  }
  int thispos = pos;
  pos += 2;
  in_ex[0] = 1;
  e1->code(s);
  in_ex[0] = 0;
  in_ex[1] = 1;
  e2->code(s);
  in_ex[1] = 0;
  emit_sub("$8", RSP, s);
  rsp -= 8;
  if (e1->getType() == Int && e2->getType() == Int)
  {
    emit_mov(sym[0], RAX, s);
    emit_mov(sym[1], RDX, s);
    emit_cmp(RDX, RAX, s);
  }
  else
  {
    if (e1->getType() == Int)
    {
      emit_mov(sym[0], RAX, s);
      emit_int_to_float(RAX, XMM1, s);
    }
    else
      emit_mov(sym[0], XMM1, s);
    if (e2->getType() == Int)
    {
      emit_mov(sym[1], RAX, s);
      emit_int_to_float(RAX, XMM0, s);
    }
    else
      emit_movsd(sym[1], XMM0, s);
    emit_ucompisd(XMM0, XMM1, s);
  }
  char* str = new char;
  sprintf(str, ".POS%d", thispos);
  emit_jle(str, s);
  emit_mov("$0", RAX, s);
  sprintf(str, ".POS%d", thispos + 1);
  emit_jmp(str, s);
  sprintf(str, ".POS%d", thispos);
  emit_position(str, s);
  emit_mov("$1", RAX, s);
  sprintf(str, ".POS%d", thispos + 1);
  emit_position(str, s);
  emit_rmmov(RAX, rsp, RBP, s);
  for (int i = 0; i < 6; i ++)
  {
    in_ex[i] = tmp_inex[i];
    strcpy(sym[i], tmp_sym[i]);
  }
  sprintf(str, "%d(%%rbp)", rsp);
  for (int i = 0;i < 6;i ++)
    if (in_ex[i])
      strcpy(sym[i], str);
}

void Equ_class::code(ostream &s) {
  bool tmp_inex[6];
  char* tmp_sym[6] = {new char, new char, new char, new char, new char, new char};
  for (int i = 0; i < 6; i ++)
  {
    tmp_inex[i] = in_ex[i];
    in_ex[i] = 0;
    strcpy(tmp_sym[i], sym[i]);
  }
  int thispos = pos;
  pos += 2;
  in_ex[0] = 1;
  e1->code(s);
  in_ex[0] = 0;
  in_ex[1] = 1;
  e2->code(s);
  in_ex[1] = 0;
  emit_sub("$8", RSP, s);
  rsp -= 8;
  if (e1->getType() == Int && e2->getType() == Int)
  {
    emit_mov(sym[0], RAX, s);
    emit_mov(sym[1], RDX, s);
    emit_cmp(RDX, RAX, s);
  }
  else
  {
    if (e1->getType() == Int)
    {
      emit_mov(sym[0], RAX, s);
      emit_int_to_float(RAX, XMM1, s);
    }
    else
      emit_mov(sym[0], XMM1, s);
    if (e2->getType() == Int)
    {
      emit_mov(sym[1], RAX, s);
      emit_int_to_float(RAX, XMM0, s);
    }
    else
      emit_movsd(sym[1], XMM0, s);
    emit_ucompisd(XMM0, XMM1, s);
  }
  char* str = new char;
  sprintf(str, ".POS%d", thispos);
  emit_je(str, s);
  emit_mov("$0", RAX, s);
  sprintf(str, ".POS%d", thispos + 1);
  emit_jmp(str, s);
  sprintf(str, ".POS%d", thispos);
  emit_position(str, s);
  emit_mov("$1", RAX, s);
  sprintf(str, ".POS%d", thispos + 1);
  emit_position(str, s);
  emit_rmmov(RAX, rsp, RBP, s);
  for (int i = 0; i < 6; i ++)
  {
    in_ex[i] = tmp_inex[i];
    strcpy(sym[i], tmp_sym[i]);
  }
  sprintf(str, "%d(%%rbp)", rsp);
  for (int i = 0;i < 6;i ++)
    if (in_ex[i])
      strcpy(sym[i], str);
}

void Neq_class::code(ostream &s) {
  bool tmp_inex[6];
  char* tmp_sym[6] = {new char, new char, new char, new char, new char, new char};
  for (int i = 0; i < 6; i ++)
  {
    tmp_inex[i] = in_ex[i];
    in_ex[i] = 0;
    strcpy(tmp_sym[i], sym[i]);
  }
  int thispos = pos;
  pos += 2;
  in_ex[0] = 1;
  e1->code(s);
  in_ex[0] = 0;
  in_ex[1] = 1;
  e2->code(s);
  in_ex[1] = 0;
  emit_sub("$8", RSP, s);
  rsp -= 8;
  if (e1->getType() == Int && e2->getType() == Int)
  {
    emit_mov(sym[0], RAX, s);
    emit_mov(sym[1], RDX, s);
    emit_cmp(RDX, RAX, s);
  }
  else
  {
    if (e1->getType() == Int)
    {
      emit_mov(sym[0], RAX, s);
      emit_int_to_float(RAX, XMM1, s);
    }
    else
      emit_mov(sym[0], XMM1, s);
    if (e2->getType() == Int)
    {
      emit_mov(sym[1], RAX, s);
      emit_int_to_float(RAX, XMM0, s);
    }
    else
      emit_movsd(sym[1], XMM0, s);
    emit_ucompisd(XMM0, XMM1, s);
  }
  char* str = new char;
  sprintf(str, ".POS%d", thispos);
  emit_jne(str, s);
  emit_mov("$0", RAX, s);
  sprintf(str, ".POS%d", thispos + 1);
  emit_jmp(str, s);
  sprintf(str, ".POS%d", thispos);
  emit_position(str, s);
  emit_mov("$1", RAX, s);
  sprintf(str, ".POS%d", thispos + 1);
  emit_position(str, s);
  emit_rmmov(RAX, rsp, RBP, s);
  for (int i = 0; i < 6; i ++)
  {
    in_ex[i] = tmp_inex[i];
    strcpy(sym[i], tmp_sym[i]);
  }
  sprintf(str, "%d(%%rbp)", rsp);
  for (int i = 0;i < 6;i ++)
    if (in_ex[i])
      strcpy(sym[i], str);
}

void Ge_class::code(ostream &s) {
  bool tmp_inex[6];
  char* tmp_sym[6] = {new char, new char, new char, new char, new char, new char};
  for (int i = 0; i < 6; i ++)
  {
    tmp_inex[i] = in_ex[i];
    in_ex[i] = 0;
    strcpy(tmp_sym[i], sym[i]);
  }
  int thispos = pos;
  pos += 2;
  in_ex[0] = 1;
  e1->code(s);
  in_ex[0] = 0;
  in_ex[1] = 1;
  e2->code(s);
  in_ex[1] = 0;
  emit_sub("$8", RSP, s);
  rsp -= 8;
  if (e1->getType() == Int && e2->getType() == Int)
  {
    emit_mov(sym[0], RAX, s);
    emit_mov(sym[1], RDX, s);
    emit_cmp(RDX, RAX, s);
  }
  else
  {
    if (e1->getType() == Int)
    {
      emit_mov(sym[0], RAX, s);
      emit_int_to_float(RAX, XMM1, s);
    }
    else
      emit_mov(sym[0], XMM1, s);
    if (e2->getType() == Int)
    {
      emit_mov(sym[1], RAX, s);
      emit_int_to_float(RAX, XMM0, s);
    }
    else
      emit_movsd(sym[1], XMM0, s);
    emit_ucompisd(XMM0, XMM1, s);
  }
  char* str = new char;
  sprintf(str, ".POS%d", thispos);
  emit_jae(str, s);
  emit_mov("$0", RAX, s);
  sprintf(str, ".POS%d", thispos + 1);
  emit_jmp(str, s);
  sprintf(str, ".POS%d", thispos);
  emit_position(str, s);
  emit_mov("$1", RAX, s);
  sprintf(str, ".POS%d", thispos + 1);
  emit_position(str, s);
  emit_rmmov(RAX, rsp, RBP, s);
  for (int i = 0; i < 6; i ++)
  {
    in_ex[i] = tmp_inex[i];
    strcpy(sym[i], tmp_sym[i]);
  }
  sprintf(str, "%d(%%rbp)", rsp);
  for (int i = 0;i < 6;i ++)
    if (in_ex[i])
      strcpy(sym[i], str);
}

void Gt_class::code(ostream &s) {
  bool tmp_inex[6];
  char* tmp_sym[6] = {new char, new char, new char, new char, new char, new char};
  for (int i = 0; i < 6; i ++)
  {
    tmp_inex[i] = in_ex[i];
    in_ex[i] = 0;
    strcpy(tmp_sym[i], sym[i]);
  }
  int thispos = pos;
  pos += 2;
  in_ex[0] = 1;
  e1->code(s);
  in_ex[0] = 0;
  in_ex[1] = 1;
  e2->code(s);
  in_ex[1] = 0;
  emit_sub("$8", RSP, s);
  rsp -= 8;
  if (e1->getType() == Int && e2->getType() == Int)
  {
    emit_mov(sym[0], RAX, s);
    emit_mov(sym[1], RDX, s);
    emit_cmp(RDX, RAX, s);
  }
  else
  {
    if (e1->getType() == Int)
    {
      emit_mov(sym[0], RAX, s);
      emit_int_to_float(RAX, XMM1, s);
    }
    else
      emit_mov(sym[0], XMM1, s);
    if (e2->getType() == Int)
    {
      emit_mov(sym[1], RAX, s);
      emit_int_to_float(RAX, XMM0, s);
    }
    else
      emit_movsd(sym[1], XMM0, s);
    emit_ucompisd(XMM0, XMM1, s);
  }
  char* str = new char;
  sprintf(str, ".POS%d", thispos);
  emit_ja(str, s);
  emit_mov("$0", RAX, s);
  sprintf(str, ".POS%d", thispos + 1);
  emit_jmp(str, s);
  sprintf(str, ".POS%d", thispos);
  emit_position(str, s);
  emit_mov("$1", RAX, s);
  sprintf(str, ".POS%d", thispos + 1);
  emit_position(str, s);
  emit_rmmov(RAX, rsp, RBP, s);
  for (int i = 0; i < 6; i ++)
  {
    in_ex[i] = tmp_inex[i];
    strcpy(sym[i], tmp_sym[i]);
  }
  sprintf(str, "%d(%%rbp)", rsp);
  for (int i = 0;i < 6;i ++)
    if (in_ex[i])
      strcpy(sym[i], str);
}

void And_class::code(ostream &s) {
  bool tmp_inex[6];
  char* tmp_sym[6] = {new char, new char, new char, new char, new char, new char};
  for (int i = 0; i < 6; i ++)
  {
    tmp_inex[i] = in_ex[i];
    in_ex[i] = 0;
    strcpy(tmp_sym[i], sym[i]);
  }
  in_ex[0] = 1;
  e1->code(s);
  in_ex[0] = 0;
  in_ex[1] = 1;
  e2->code(s);
  in_ex[1] = 0;
  emit_sub("$8", RSP, s);
  rsp -= 8;
  emit_mov(sym[0], RAX, s);
  emit_mov(sym[1], RDX, s);
  emit_and(RAX, RDX, s);
  emit_rmmov(RDX, rsp, RBP, s);
  for (int i = 0; i < 6; i ++)
  {
    in_ex[i] = tmp_inex[i];
    strcpy(sym[i], tmp_sym[i]);
  }
  char* str = new char;
  sprintf(str, "%d(%%rbp)", rsp);
  for (int i = 0;i < 6;i ++)
    if (in_ex[i])
      strcpy(sym[i], str);
}

void Or_class::code(ostream &s) {
  bool tmp_inex[6];
  char* tmp_sym[6] = {new char, new char, new char, new char, new char, new char};
  for (int i = 0; i < 6; i ++)
  {
    tmp_inex[i] = in_ex[i];
    in_ex[i] = 0;
    strcpy(tmp_sym[i], sym[i]);
  }
  in_ex[0] = 1;
  e1->code(s);
  in_ex[0] = 0;
  in_ex[1] = 1;
  e2->code(s);
  in_ex[1] = 0;
  emit_sub("$8", RSP, s);
  rsp -= 8;
  emit_mov(sym[0], RAX, s);
  emit_mov(sym[1], RDX, s);
  emit_or(RAX, RDX, s);
  emit_rmmov(RDX, rsp, RBP, s);
  for (int i = 0; i < 6; i ++)
  {
    in_ex[i] = tmp_inex[i];
    strcpy(sym[i], tmp_sym[i]);
  }
  char* str = new char;
  sprintf(str, "%d(%%rbp)", rsp);
  for (int i = 0;i < 6;i ++)
    if (in_ex[i])
      strcpy(sym[i], str);
}

void Xor_class::code(ostream &s) {
  bool tmp_inex[6];
  char* tmp_sym[6] = {new char, new char, new char, new char, new char, new char};
  for (int i = 0; i < 6; i ++)
  {
    tmp_inex[i] = in_ex[i];
    in_ex[i] = 0;
    strcpy(tmp_sym[i], sym[i]);
  }
  in_ex[0] = 1;
  e1->code(s);
  in_ex[0] = 0;
  in_ex[1] = 1;
  e2->code(s);
  in_ex[1] = 0;
  emit_sub("$8", RSP, s);
  rsp -= 8;
  emit_mov(sym[0], RAX, s);
  emit_mov(sym[1], RDX, s);
  emit_xor(RAX, RDX, s);
  emit_rmmov(RDX, rsp, RBP, s);
  for (int i = 0; i < 6; i ++)
  {
    in_ex[i] = tmp_inex[i];
    strcpy(sym[i], tmp_sym[i]);
  }
  char* str = new char;
  sprintf(str, "%d(%%rbp)", rsp);
  for (int i = 0;i < 6;i ++)
    if (in_ex[i])
      strcpy(sym[i], str);
}

void Not_class::code(ostream &s) {
  bool tmp_inex[6];
  char* tmp_sym[6] = {new char, new char, new char, new char, new char, new char};
  for (int i = 0; i < 6; i ++)
  {
    tmp_inex[i] = in_ex[i];
    in_ex[i] = 0;
    strcpy(tmp_sym[i], sym[i]);
  }
  in_ex[0] = 1;
  e1->code(s);
  in_ex[0] = 0;
  emit_sub("$8", RSP, s);
  rsp -= 8;
  emit_mov(sym[0], RAX, s);
  emit_mov("$0x0000000000000001", RDX, s);
  emit_xor(RDX, RAX, s);
  emit_rmmov(RAX, rsp, RBP, s);
  for (int i = 0; i < 6; i ++)
  {
    in_ex[i] = tmp_inex[i];
    strcpy(sym[i], tmp_sym[i]);
  }
  char* str = new char;
  sprintf(str, "%d(%%rbp)", rsp);
  for (int i = 0;i < 6;i ++)
    if (in_ex[i])
      strcpy(sym[i], str);
}

void Bitnot_class::code(ostream &s) {
  bool tmp_inex[6];
  char* tmp_sym[6] = {new char, new char, new char, new char, new char, new char};
  for (int i = 0; i < 6; i ++)
  {
    tmp_inex[i] = in_ex[i];
    in_ex[i] = 0;
    strcpy(tmp_sym[i], sym[i]);
  }
  in_ex[0] = 1;
  e1->code(s);
  in_ex[0] = 0;
  emit_sub("$8", RSP, s);
  rsp -= 8;
  emit_mov(sym[0], RAX, s);
  emit_not(RAX, s);
  emit_rmmov(RAX, rsp, RBP, s);
  for (int i = 0; i < 6; i ++)
  {
    in_ex[i] = tmp_inex[i];
    strcpy(sym[i], tmp_sym[i]);
  }
  char* str = new char;
  sprintf(str, "%d(%%rbp)", rsp);
  for (int i = 0;i < 6;i ++)
    if (in_ex[i])
      strcpy(sym[i], str);
}

void Bitand_class::code(ostream &s) {
  bool tmp_inex[6];
  char* tmp_sym[6] = {new char, new char, new char, new char, new char, new char};
  for (int i = 0; i < 6; i ++)
  {
    tmp_inex[i] = in_ex[i];
    in_ex[i] = 0;
    strcpy(tmp_sym[i], sym[i]);
  }
  in_ex[0] = 1;
  e1->code(s);
  in_ex[0] = 0;
  in_ex[1] = 1;
  e2->code(s);
  in_ex[1] = 0;
  emit_sub("$8", RSP, s);
  rsp -= 8;
  emit_mov(sym[0], RAX, s);
  emit_mov(sym[1], RDX, s);
  emit_and(RAX, RDX, s);
  emit_rmmov(RDX, rsp, RBP, s);
  for (int i = 0; i < 6; i ++)
  {
    in_ex[i] = tmp_inex[i];
    strcpy(sym[i], tmp_sym[i]);
  }
  char* str = new char;
  sprintf(str, "%d(%%rbp)", rsp);
  for (int i = 0;i < 6;i ++)
    if (in_ex[i])
      strcpy(sym[i], str);
}

void Bitor_class::code(ostream &s) {
  bool tmp_inex[6];
  char* tmp_sym[6] = {new char, new char, new char, new char, new char, new char};
  for (int i = 0; i < 6; i ++)
  {
    tmp_inex[i] = in_ex[i];
    in_ex[i] = 0;
    strcpy(tmp_sym[i], sym[i]);
  }
  in_ex[0] = 1;
  e1->code(s);
  in_ex[0] = 0;
  in_ex[1] = 1;
  e2->code(s);
  in_ex[1] = 0;
  emit_sub("$8", RSP, s);
  rsp -= 8;
  emit_mov(sym[0], RAX, s);
  emit_mov(sym[1], RDX, s);
  emit_or(RAX, RDX, s);
  emit_rmmov(RDX, rsp, RBP, s);
  for (int i = 0; i < 6; i ++)
  {
    in_ex[i] = tmp_inex[i];
    strcpy(sym[i], tmp_sym[i]);
  }
  char* str = new char;
  sprintf(str, "%d(%%rbp)", rsp);
  for (int i = 0;i < 6;i ++)
    if (in_ex[i])
      strcpy(sym[i], str);
}

void Const_int_class::code(ostream &s) {
  emit_sub("$8", RSP, s);
  rsp -= 8;
  char *str = new char;
  strcpy(str, "$");
  strcat(str, value->get_string());
  emit_mov(str, RAX, s);
  emit_rmmov(RAX, rsp, RBP, s);
  sprintf(str, "%d(%%rbp)", rsp);
  for (int i = 0;i < 6;i ++)
    if (in_ex[i])
      strcpy(sym[i], str);
}

void Const_string_class::code(ostream &s) {
  emit_sub("$8", RSP, s);
  rsp -= 8;
  s << MOV;
  int index;
  for (int i = stringtable.first(); stringtable.more(i); i = stringtable.next(i))
    if (stringtable.lookup(i)->get_string() == value->get_string())
      index = i;
  (StringEntry(value->get_string(), value->get_len(), index)).code_ref(s);
  s << COMMA << RAX << endl;
  emit_rmmov(RAX, rsp, RBP, s);
  char *str = new char;
  sprintf(str, "%d(%%rbp)", rsp);
  for (int i = 0;i < 6;i ++)
    if (in_ex[i])
      strcpy(sym[i], str);
}

void Const_float_class::code(ostream &s) {
  emit_sub("$8", RSP, s);
  rsp -= 8;
  double val = atof(value->get_string());
  char *str = new char, *s0 = new char;
  strcpy(str, "$0x");
  for(int i=sizeof(val)-1;i>=0;i--)
  {
    sprintf(s0, "%02x", ((unsigned char*)&val)[i]);
    strcat(str, s0);
  }
  emit_mov(str, RAX, s);
  emit_rmmov(RAX, rsp, RBP, s);
  sprintf(str, "%d(%%rbp)", rsp);
  for (int i = 0;i < 6;i ++)
    if (in_ex[i])
      strcpy(sym[i], str);
}

void Const_bool_class::code(ostream &s) {
  emit_sub("$8", RSP, s);
  rsp -= 8;
  char *str = new char;
  if (value)
    strcpy(str, "$1");
  else
    strcpy(str, "$0");
  emit_mov(str, RAX, s);
  emit_rmmov(RAX, rsp, RBP, s);
  sprintf(str, "%d(%%rbp)", rsp);
  for (int i = 0;i < 6;i ++)
    if (in_ex[i])
      strcpy(sym[i], str);
}

void Object_class::code(ostream &s) {
  for (int i=0; i<6; i++)
  {
    if (in_ex[i])
    {
      if (objectEnv00.probe(var) != NULL)
        strcpy(sym[i], objectEnv00.probe(var));
      else if (objectEnv00.lookup(var) != NULL)
        strcpy(sym[i], objectEnv00.lookup(var));
      else
      {
        strcpy(sym[i], var->get_string());
        strcat(sym[i], "(");
        strcat(sym[i], RIP);
        strcat(sym[i], ")");
      }
    }
  }
}

void No_expr_class::code(ostream &s) {

}