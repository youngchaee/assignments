// B2CMain.cpp
#include <iostream>
#include <map>
#include <stack>
#include "antlr4-runtime.h"
#include "antlr4-cpp/BBaseVisitor.h"
#include "antlr4-cpp/BLexer.h"
#include "antlr4-cpp/BParser.h"

using namespace std;
using namespace antlr4;
using namespace antlr4::tree;

enum Types {tyAUTO, tyINT, tyDOUBLE, tySTRING, tyBOOL, tyCHAR, tyFUNCTION, tyVOID,tyUNDEFINED};
string mnemonicTypes[] = {"auto", "int", "double", "string", "bool", "char", "function","void"};

struct SymbolAttributes {
   Types type; // int, double, bool, char, string, function --- auto if unknown yet

   // if type == "function"
   vector<Types> retArgTypes; // first element is a return_type
};

class SymbolTable {
private:
    map<string, SymbolAttributes> table;  // symbol-name: string, symbol-typeInfo: SymbolAttributes

public:
    // Add a new symbol 
    void addSymbol(const string& name, const SymbolAttributes& attributes) {
		table[name] = attributes; 
    }

    // Check if a symbol exists
    bool symbolExists(const string& name) const {
        return table.find(name) != table.end();
    }

    // Get attributes of a symbol
    SymbolAttributes getSymbolAttributes(const string& name) const {
        if (symbolExists(name)) {
            return table.at(name);
        } else {
            cout << "Error: Symbol " << name << " not found" << endl;
            exit(-1);
        }
    }

    // Remove a symbol from the table
    void removeSymbol(const string& name) {
        table.erase(name);
    }
    bool autoexist() {
    for ( auto& pair : table) {
    if(pair.second.type==tyAUTO){pair.second.type={tyUNDEFINED};
    }
    if(pair.second.type==tyFUNCTION){
    int n = pair.second.retArgTypes.size();
    for (int i = 0; i < n; i++){
    if(pair.second.retArgTypes[i]==tyAUTO){

    pair.second.retArgTypes[i]={tyUNDEFINED};
    }}
    }
    }
    return false;
    }
    // Print all symbols in the table (for debugging purposes)
    void printSymbols() const {
        for (const auto& pair : table) {
            cout << "(name) " << pair.first << ", (type) " << mnemonicTypes[pair.second.type];
			if (pair.second.type == tyFUNCTION) {
				cout << "| ";
				int n = pair.second.retArgTypes.size();
				if (n > 0) {
					cout << mnemonicTypes[pair.second.retArgTypes[0]] << "("; // return type
				}
				for (int i = 1; i < n-1; i++)
					cout << mnemonicTypes[pair.second.retArgTypes[i]] << ", ";
				if (n > 1) {
					cout << mnemonicTypes[pair.second.retArgTypes[n-1]]; // last arg type
				}
				cout << ")";
			} 
	    	cout << endl;
        }
    }
};



/*
 * STEP 1. build symbol table
 */

      
const string _GlobalFuncName_ = "$_global_$";

// collection of per-function symbol tables accessed by function name
// symbol table in global scope can be accessed with special name defined in _GlobalFuncName_
map<string, SymbolTable*> symTabs;
string findfuncname(int scopeLevel, string curFuncName, int curnum){
if(scopeLevel<2&&curFuncName.find("_$")==string::npos){
              curFuncName+="_$";
            }
            else if(scopeLevel>1){
              int namelength=curFuncName.length();
              int numof_=0;
              for(int i=0;i<namelength;i++){
              if(curFuncName.at(i)=='_'){
              numof_++;
              }}
              if(numof_<scopeLevel){
              curFuncName+="_";}}
              if(isdigit(curFuncName.back())){
              curFuncName.pop_back();}
                curFuncName+=to_string(curnum);
              
              return curFuncName;}

class SymbolTableVisitor : public BBaseVisitor {
private:
   int scopeLevel;
   string curFuncName;
   int numofblock;
public:
	// Building symbol tables by visiting tree

	
	any visitProgram(BParser::ProgramContext *ctx) override {
		scopeLevel = 0; // global scope	
			numofblock=0;
		// prepare symbol table for global scope
		SymbolTable* globalSymTab = new SymbolTable();
		curFuncName = _GlobalFuncName_;
    	symTabs[curFuncName] = globalSymTab;
    	
    	
    		// visit children
    	for (int i=0; i< ctx->definition().size(); i++) {
    	        if(ctx->definition(i)->declstmt()){
    		visit(ctx->definition(i));
    		numofblock=0;}
    	}
    	for (int i=0; i< ctx->definition().size(); i++) {
    	        if(!(ctx->definition(i)->declstmt())){
    		visit(ctx->definition(i));}
    	}

		// print all symbol tables
		for (auto& pair : symTabs) {
	    	cout << "--- Symbol Table --- " << pair.first << endl; // function name
	    	pair.second->printSymbols();					   // per-function symbol table
			cout << "";
		}

    	return nullptr;
	}
   any visitDeclstmt(BParser::DeclstmtContext *ctx) override {
   SymbolTable *stab = symTabs[curFuncName];
    enum Types varType = tyFUNCTION;
    if(stab->symbolExists(ctx->name()->getText())){
    int lineNum = ctx->getStart()->getLine();
	cerr << endl << "[ERROR] Multiple function definition in Line " << lineNum << " --" << ctx->children[1]->getText() << endl;
        exit(-1); // error
      
    }
    else{
    vector<Types> functypes;
    for(int i=0;i<ctx->AUTO().size();i++){
    functypes.push_back(tyAUTO);
    }
    stab->addSymbol(ctx->name()->getText(),{varType,functypes});
    
    }
    return nullptr;
   }
    
    any visitDefinition(BParser::DefinitionContext *ctx) override {
    //
    scopeLevel=0;
    curFuncName = _GlobalFuncName_;
		visit(ctx->children[0]);
        return nullptr;
	}

    any visitAutostmt(BParser::AutostmtContext *ctx) override {
    	// get current symbol table
		SymbolTable *stab = symTabs[curFuncName];

		// You can retrieve the variable names and constants using ctx->name(i) and ctx->constant(i)
		for (int i=0, j=0; i < ctx->name().size(); i++) {
			
			string varName = ctx->name(i)->getText();
			enum Types varType = tyAUTO;				// default type

			// if initialized, get constant type
			int idx_assn = 1 + i*2 + j*2 + 1;  // auto name (= const)?, name (= const)?, ...
			if (ctx->children[idx_assn]->getText().compare("=") == 0) { 
				if (ctx->constant(j)) {  
					varType = any_cast<Types>( visit(ctx->constant(j)) );   // returns init constant type
					j++;
				}
			}
                        if(stab->symbolExists(varName)){
    int lineNum = ctx->getStart()->getLine();
	cerr << endl << "[ERROR] Multiple variable definition in Line " << lineNum << " --" << varName << endl;
        exit(-1); // error
      
    }
			stab->addSymbol(varName, {varType});
		}
    	return nullptr;
    }
    any visitFuncdef(BParser::FuncdefContext *ctx) override {
    
    SymbolTable *stab = symTabs[curFuncName];
    enum Types varType = tyFUNCTION;
    if(stab->symbolExists(ctx->name(0)->getText())){
    if(symTabs.count(ctx->name(0)->getText())){
    int lineNum = ctx->getStart()->getLine();
	  cerr << endl << "[ERROR] Multiple function definition in Line " << lineNum << " --" << ctx->name(0)->getText()<< endl;
	  exit(-1); // error
    }
    }
    else{
    vector<Types> functypes;
    for(int i=0;i<ctx->AUTO().size();i++){
    functypes.push_back(tyAUTO);
    }
    stab->addSymbol(ctx->name(0)->getText(),{varType,functypes});}
    
    SymbolTable* funcSymTab = new SymbolTable();
		curFuncName =ctx->name(0)->getText();
    	symTabs[curFuncName] = funcSymTab;
    	
        for(int i=1;i<ctx->name().size();i++){
         if(funcSymTab->symbolExists(ctx->name(i)->getText())){
    int lineNum = ctx->getStart()->getLine();
	cerr << endl << "[ERROR] Multiple variable definition in Line " << lineNum << " --" << ctx->name(i)->getText() << endl;
        exit(-1); // error
    }
        funcSymTab->addSymbol(ctx->name(i)->getText(),{tyAUTO});
        }
        visit(ctx->blockstmt());
        return nullptr;
    }

    any visitStatement(BParser::StatementContext *ctx) override {
          int curlevel=scopeLevel;
          string curfunc=curFuncName;
          if(ctx->blockstmt()){
            int curnum=++numofblock;
            
            curFuncName=findfuncname(scopeLevel, curFuncName,curnum);
          SymbolTable* funcSymTab = new SymbolTable();
          symTabs[curFuncName] = funcSymTab;
          
    	        
    		visit(ctx->blockstmt());
    numofblock=curnum;
    curFuncName=curfunc;
    return nullptr;
          }
   
    visitChildren(ctx);
    scopeLevel=curlevel;
    return nullptr;
    }
    any visitBlockstmt(BParser::BlockstmtContext *ctx) override {
          scopeLevel++;
          
          int curlevel=scopeLevel;
          int curnum=++numofblock;
          numofblock=0;
          string curFuncName_=curFuncName;
          for (int i=0; i< ctx->statement().size(); i++) {
          
                scopeLevel=curlevel;
                curFuncName=curFuncName_;
    		visit(ctx->statement(i));
    	}
    	numofblock=curnum;
    return nullptr;
    }
    any visitIfstmt(BParser::IfstmtContext *ctx) override {
          string curfunc=curFuncName;
          int curlevel=scopeLevel;
          for (int i=0; i< ctx->statement().size(); i++) {
          if(!(ctx->statement(i)->blockstmt())){
           int curnum=++numofblock;
            
            curFuncName=findfuncname(scopeLevel, curFuncName,curnum);
          SymbolTable* funcSymTab = new SymbolTable();
          symTabs[curFuncName] = funcSymTab;
          
    	        scopeLevel++;
    	        numofblock=0;
    		visit(ctx->statement(i));   		
    numofblock=curnum;
    scopeLevel=curlevel;
    curFuncName=curfunc;
    	}
    	else{visit(ctx->statement(i));
    scopeLevel=curlevel;
    curFuncName=curfunc;
    }}
    return nullptr;
    }
    any visitWhilestmt(BParser::WhilestmtContext *ctx) override {
    string curfunc=curFuncName;
    int curlevel=scopeLevel;
    if(!(ctx->statement()->blockstmt())){
    int curnum=++numofblock;
            
            curFuncName=findfuncname(scopeLevel, curFuncName,curnum);
          SymbolTable* funcSymTab = new SymbolTable();
          symTabs[curFuncName] = funcSymTab;
          
    	        scopeLevel++;
    	        numofblock=0;
    		visit(ctx->statement());
    		
    numofblock=curnum;
    scopeLevel=curlevel;
    curFuncName=curfunc;
    return nullptr;
    }
    
    visit(ctx->statement());
    scopeLevel=curlevel;
    curFuncName=curfunc;
    return nullptr;
    }
    any visitConstant(BParser::ConstantContext *ctx) override {
        
		if (ctx->INT()) return tyINT;
		else if (ctx->REAL()) return tyDOUBLE;
		else if (ctx->STRING()) return tySTRING;
		else if (ctx->BOOL()) return tyBOOL;
		else if (ctx->CHAR()) return tyCHAR;

		cout << "[ERROR] unrecognizable constant is used for initialization: " << ctx->children[0]->getText() << endl;
		exit(-1);
        return nullptr;
    }

};

/* 
 * STEP 2. infer type
 */   
 
 bool change;
class TypeAnalysisVisitor : public BBaseVisitor {
private:
   int scopeLevel;
   string curFuncName;
   int numofblock;
   
public:
  any visitProgram(BParser::ProgramContext *ctx) override {
		scopeLevel = 0; // global scope	
numofblock=0;

		curFuncName = _GlobalFuncName_;
    		// visit children
    	for (int i=0; i< ctx->definition().size(); i++) {
    	        
    		visit(ctx->definition(i));
    	}
    	
    	 return nullptr;}
    	
    	
      any visitDefinition(BParser::DefinitionContext *ctx) override {
    scopeLevel=0;
    curFuncName = _GlobalFuncName_;
		visit(ctx->children[0]);
        return nullptr;
	}
	
      any visitDeclstmt(BParser::DeclstmtContext *ctx) override {
	SymbolTable *stab = symTabs[curFuncName];
	if(symTabs[curFuncName]->getSymbolAttributes(ctx->name()->getText()).type==tyUNDEFINED){
	int lineNum = ctx->getStart()->getLine();
	cerr << endl << "[ERROR] type undefined in line " << lineNum << endl;
      exit(-1);
      }
      return nullptr;
	}
	
      any visitAutostmt(BParser::AutostmtContext *ctx) override {
      SymbolTable *stab = symTabs[curFuncName];
      int Size=ctx->name().size();
      for(int i=0;i<Size;i++){
	if(symTabs[curFuncName]->getSymbolAttributes(ctx->name(i)->getText()).type==tyUNDEFINED){
	int lineNum = ctx->getStart()->getLine();
	cerr << endl << "[ERROR] type undefined in line " << lineNum<< " --" << ctx->name(i)->getText() << endl;
      exit(-1);
      }}
      return nullptr;}
      
      any visitStatement(BParser::StatementContext *ctx) override {
          int curlevel=scopeLevel;
          string curfunc=curFuncName;
          if(ctx->blockstmt()){
            int curnum=++numofblock;
            curFuncName=findfuncname(scopeLevel, curFuncName,curnum);       
    		visit(ctx->blockstmt());
    numofblock=curnum;
    curFuncName=curfunc;
    return nullptr;
          }
    visitChildren(ctx);
    scopeLevel=curlevel;
    return nullptr;
    }
	
    any visitFuncdef(BParser::FuncdefContext *ctx) override {
    
    SymbolTable *stab = symTabs[curFuncName];
    curFuncName =ctx->name(0)->getText();
    visit(ctx->blockstmt());
    curFuncName =ctx->name(0)->getText();
    SymbolAttributes funsym = stab->getSymbolAttributes(curFuncName);
    enum Types retType=funsym.retArgTypes[0];
    bool isvoid=true;
    int Size=funsym.retArgTypes.size();
    for(int i=0;i<Size;i++){
    if(funsym.retArgTypes[i]==tyUNDEFINED){int lineNum = ctx->getStart()->getLine();
    if(i==0) cerr << endl << "[ERROR] return type undefined in line " <<lineNum << " --" << curFuncName<< endl;
	else cerr << endl << "[ERROR] type undefined in line " << lineNum <<" --" << ctx->name(i)->getText()<< endl;
      exit(-1);
      }}
for(int i=1;i<Size;i++){
if(funsym.retArgTypes[i]!=tyAUTO){
Types vartype=symTabs[curFuncName]->getSymbolAttributes(ctx->name(i)->getText()).type;
if(vartype!=funsym.retArgTypes[i]){
symTabs[curFuncName]->removeSymbol(ctx->name(i)->getText());
  symTabs[curFuncName]->addSymbol(ctx->name(i)->getText(),{funsym.retArgTypes[i]});
  change=true;
}}}
    for(int i=0;i<ctx->blockstmt()->statement().size();i++){
    if((ctx->blockstmt()->statement(i)->returnstmt())){
      enum Types ret=any_cast<Types>(visit(ctx->blockstmt()->statement(i)->returnstmt()));
      isvoid=false;
      if(retType==tyAUTO&&retType!=ret){retType=ret; change=true;}
      else if(retType!=ret&&retType!=tyAUTO){

    int lineNum = ctx->getStart()->getLine();
	cerr << endl << "[ERROR] return type error " << lineNum << " --" << ctx->children[1]->getText() << endl;
      exit(-1);}
      
    }}
    if(isvoid){retType=tyVOID;}
    stab->removeSymbol(curFuncName);
    funsym.retArgTypes[0]=retType;
    stab->addSymbol(curFuncName,funsym);
    
        return nullptr;
    }
    
    
any visitBlockstmt(BParser::BlockstmtContext *ctx) override {
          scopeLevel++;
          int curlevel=scopeLevel;
          int curnum=++numofblock;
          numofblock=0;
          string curFuncName_=curFuncName;
          for (int i=0; i< ctx->statement().size(); i++) {
                scopeLevel=curlevel;
                curFuncName=curFuncName_;
    		visit(ctx->statement(i));
    	}
    	numofblock=curnum;
    return nullptr;
    }

any visitReturnstmt(BParser::ReturnstmtContext *ctx) override {
if(ctx->expression())
    return any_cast<Types> (visit(ctx->expression()));
else{

return tyVOID;}
}
   

  any visitExpression(BParser::ExpressionContext *ctx) override {

  if(ctx->name()){
 string func=curFuncName;
    int curscope=scopeLevel;
    string Name=ctx->name()->getText();
    SymbolTable *stab = symTabs[curFuncName];
    while(!(stab->symbolExists(Name))){
      curscope--;
      if(curscope<0){
      int lineNum = ctx->getStart()->getLine();
	cerr << endl << "[ERROR] variable does not exist at line" << lineNum << " --" << ctx->name()->getText() << endl;
    exit(-1);
      }
      if(curscope==0){func=_GlobalFuncName_;
      }
      else{while(func.back()!='_'){
      func.pop_back();
      }func.pop_back();}
      stab=symTabs[func];
    }
  SymbolAttributes sym = stab->getSymbolAttributes(ctx->name()->getText());
  enum Types exprtype=any_cast<Types>(visit(ctx->expr()));
  if(exprtype!=tyAUTO&&sym.type!=tyAUTO&&exprtype!=sym.type){int lineNum = ctx->getStart()->getLine();
	cerr << endl << "[ERROR] type mismatch at line" << lineNum << " --" << ctx->name()->getText() << endl;
    exit(-1);}
  else if(sym.type==tyAUTO&&sym.type!=exprtype){
  
  sym.type=exprtype;
  stab->removeSymbol(ctx->name()->getText());
  stab->addSymbol(ctx->name()->getText(),sym);
  change=true; 
  }}
    return any_cast<Types>(visit(ctx->expr()));
    }
    
    
    
  any visitExpr(BParser::ExprContext *ctx) override {
    if(ctx->atom()){
    return any_cast<Types>(visit(ctx->atom()));}
    
    enum Types type1=any_cast<Types>(visit(ctx->expr(0))), type2=any_cast<Types>(visit(ctx->expr(1)));
    if(ctx->QUEST()){type1=type2;
    type2=any_cast<Types>(visit(ctx->expr(2)));}
    if((type1!=type2&&(type1!=tyAUTO&&type2!=tyAUTO))){
    int lineNum = ctx->getStart()->getLine();
	cerr << endl << "[ERROR] expr type error at line" << lineNum << endl;
    exit(-1);}
if(ctx->MUL()||ctx->DIV()||ctx->PLUS()||ctx->MINUS()||ctx->QUEST()){
    return type1!=tyAUTO? type1:type2;}
    else{return tyBOOL;}
  
   }
    
    
    any visitAtom(BParser::AtomContext *ctx) override {
    if(ctx->name()){
    string func=curFuncName;
    int curscope=scopeLevel;
    string Name=ctx->name()->getText();
    SymbolTable *stab = symTabs[curFuncName];
    while(!(stab->symbolExists(Name))){
      curscope--;
      if(curscope<0){
      int lineNum = ctx->getStart()->getLine();
	cerr << endl << "[ERROR] variable does not exist at line" << lineNum << " --" << Name << endl;
    exit(-1);
      }
      if(curscope==0){func=_GlobalFuncName_;
      }
      else{while(func.back()!='_'){
      func.pop_back();
      }func.pop_back();}
      stab=symTabs[func];
    }
    return stab->getSymbolAttributes(ctx->name()->getText()).type;}
    
    
    else if(ctx->constant()){return any_cast<Types> (visit(ctx->constant()));}
    else if(ctx->expression()){return any_cast<Types> (visit(ctx->expression()->expr()));}
    else if(ctx->funcinvocation()){return any_cast<Types> (visit(ctx->funcinvocation()));}
    else{return tyAUTO;}
    }
    
    
    any visitIfstmt(BParser::IfstmtContext *ctx) override {
    visit(ctx->expr());
          string curfunc=curFuncName;
          int curlevel=scopeLevel;
          for (int i=0; i< ctx->statement().size(); i++) {
          if(!(ctx->statement(i)->blockstmt())){
           int curnum=++numofblock;
            curFuncName=findfuncname(scopeLevel, curFuncName,curnum);
    	        scopeLevel++;
    	        numofblock=0;
    		visit(ctx->statement(i));   		
    numofblock=curnum;
    scopeLevel=curlevel;
    curFuncName=curfunc;
    	}
    	else{visit(ctx->statement(i));
    scopeLevel=curlevel;
    curFuncName=curfunc;
    }}
    return nullptr;
    }
   any visitWhilestmt(BParser::WhilestmtContext *ctx) override {
   visit(ctx->expr());
    string curfunc=curFuncName;
    int curlevel=scopeLevel;
    if(!(ctx->statement()->blockstmt())){
    int curnum=++numofblock;       
            curFuncName=findfuncname(scopeLevel, curFuncName,curnum);
    	        scopeLevel++;
    	        numofblock=0;
    		visit(ctx->statement());
    		
    numofblock=curnum;
    scopeLevel=curlevel;
    curFuncName=curfunc;
    return nullptr;
    }
    
    visit(ctx->statement());
    scopeLevel=curlevel;
    curFuncName=curfunc;
    return nullptr;
    }
    
    any visitFuncinvocation(BParser::FuncinvocationContext *ctx){
SymbolTable *stab = symTabs[_GlobalFuncName_];
string Name=ctx->name()->getText();
if(stab->symbolExists(Name)){

SymbolAttributes funsym = stab->getSymbolAttributes(Name);
if(funsym.type!=tyFUNCTION||funsym.retArgTypes.size()!=ctx->expr().size()+1){int lineNum = ctx->getStart()->getLine();

cerr << endl << "[ERROR] argument size error " << lineNum << endl;
    exit(-1);}
if(funsym.retArgTypes.size()>1){
int Size=funsym.retArgTypes.size()-1;

for(int i=0;i<Size;i++){
Types argtype=any_cast<Types>(visit(ctx->expr(i)));
if(funsym.retArgTypes[i+1]!=tyAUTO&&argtype!=funsym.retArgTypes[i+1]&&argtype!=tyAUTO){int lineNum = ctx->getStart()->getLine();

cerr << endl << "[ERROR] argument type error " << lineNum << " --" << ctx->expr(i)->getText()<< endl;
    exit(-1);}
else if(argtype!=funsym.retArgTypes[i+1]&&funsym.retArgTypes[i+1]==tyAUTO){funsym.retArgTypes[i+1]=argtype;

change=true;}}
    
}
stab->removeSymbol(Name);
  stab->addSymbol(Name,funsym);
    return funsym.retArgTypes[0];}
else{
    return tyAUTO;}

}
any visitConstant(BParser::ConstantContext *ctx) override {
        
		if (ctx->INT()) return tyINT;
		else if (ctx->REAL()) return tyDOUBLE;
		else if (ctx->STRING()) return tySTRING;
		else if (ctx->BOOL()) return tyBOOL;
		else if (ctx->CHAR()) return tyCHAR;

		cout << "[ERROR] unrecognizable constant is used for initialization: " << ctx->children[0]->getText() << endl;
		exit(-1);
        return nullptr;
    }
   // infer types for 'auto' variables and functions
   // ...
  
};


/*
 * STEP 3. print code
 */
class PrintTreeVisitor : public BBaseVisitor {
private:
  int scopeLevel;
   string curFuncName;
   int numofblock;
public:
    any visitProgram(BParser::ProgramContext *ctx) override {
    
    curFuncName = _GlobalFuncName_;
    	// Perform some actions when visiting the program
    	for (int i=0; i< ctx->children.size(); i++) {
    	 scopeLevel=0;
    curFuncName = _GlobalFuncName_;
      	    visit(ctx->children[i]);
    	}
    	return nullptr;
    }
    
    any visitDirective(BParser::DirectiveContext *ctx) override {
		cout << ctx->SHARP_DIRECTIVE()->getText();
		cout << endl;
        return nullptr;
    }

    any visitDefinition(BParser::DefinitionContext *ctx) override {
		visit(ctx->children[0]);
        return nullptr;
    }

    any visitFuncdef(BParser::FuncdefContext *ctx) override {
		// Handle function definition
        string functionName = ctx->name(0)->getText();
        SymbolTable *stab = symTabs[curFuncName];
        SymbolAttributes att=stab->getSymbolAttributes(functionName);
		cout << mnemonicTypes[att.retArgTypes[0]]<<' ' << functionName << "(" ;
        // You can retrieve and visit the parameter list using ctx->name(i)
		for (int i=1; i < ctx->name().size(); i++) {
			if (i != 1) cout << ", ";
			cout << mnemonicTypes[att.retArgTypes[i]]<< ' ' << ctx->name(i)->getText();		
		}
		cout << ")";
		curFuncName=functionName;

		// visit blockstmt
		visit(ctx->blockstmt());
        return nullptr;
    }

    any visitStatement(BParser::StatementContext *ctx) override {
    int curlevel=scopeLevel;
          string curfunc=curFuncName;
          if(ctx->blockstmt()){
            int curnum=++numofblock;
            curFuncName=findfuncname(scopeLevel, curFuncName,curnum);       
    		visit(ctx->blockstmt());
    numofblock=curnum;
    curFuncName=curfunc;
    return nullptr;
          }
    visitChildren(ctx);
    scopeLevel=curlevel;
    return nullptr;
    }

    any visitAutostmt(BParser::AutostmtContext *ctx) override {
    	// You can retrieve the variable names and constants using ctx->name(i) and ctx->constant(i)
    	SymbolTable *stab = symTabs[curFuncName];
 
		for (int i=0, j=0; i < ctx->name().size(); i++) {
			SymbolAttributes att=stab->getSymbolAttributes(ctx->name(i)->getText());
			cout << mnemonicTypes[att.type]<<' ';
			cout << ctx->name(i)->getText();

			int idx_assn = 1 + i*2 + j*2 + 1;  // auto name (= const)?, name (= const)?, ...
			if (ctx->children[idx_assn]->getText().compare("=") == 0) { 
				if (ctx->constant(j)) {
					cout << " = ";    
					visit(ctx->constant(j));
					j++;
				}
			}
			cout << ";" << endl;
		}
		
    	return nullptr;
    }

    any visitDeclstmt(BParser::DeclstmtContext *ctx) override {
		// Handle function declaration
        string functionName = ctx->name()->getText();
        SymbolTable *stab = symTabs[curFuncName];
        SymbolAttributes att=stab->getSymbolAttributes(functionName);
		cout << mnemonicTypes[att.retArgTypes[0]]<<' ' << functionName << "(" ;
        
		// You can retrieve and visit the parameter type list
		for (int i=1; i < ctx->AUTO().size(); i++) {
			if (i != 1) cout << ", ";
			cout << mnemonicTypes[att.retArgTypes[i]]<<' ';		
		}
		cout << ");" << endl;
        return nullptr;
    }

    any visitBlockstmt(BParser::BlockstmtContext *ctx) override {
    	// Perform some actions when visiting a block statement
    	scopeLevel++;
          int curlevel=scopeLevel;
          int curnum=++numofblock;
          numofblock=0;
          string curFuncName_=curFuncName;
		cout << "{" << endl;
    	for (auto stmt : ctx->statement()) {
    	scopeLevel=curlevel;
                curFuncName=curFuncName_;
      	    visit(stmt);
    	}
		cout << "}" << endl;
		numofblock=curnum;
    	return nullptr;
    }

    any visitIfstmt(BParser::IfstmtContext *ctx) override {
    string curfunc=curFuncName;
          int curlevel=scopeLevel;
		cout << "if (";
		visit(ctx->expr());
		cout << ") " ;
if(!(ctx->statement(0)->blockstmt())){
           int curnum=++numofblock;
            curFuncName=findfuncname(scopeLevel, curFuncName,curnum);
    	        scopeLevel++;
    	        numofblock=0;
    		visit(ctx->statement(0));   		
    numofblock=curnum;
    scopeLevel=curlevel;
    curFuncName=curfunc;
    	}
    	else{visit(ctx->statement(0));
    scopeLevel=curlevel;
    curFuncName=curfunc;
    }
		if (ctx->ELSE()) {
	   		cout << endl << "else ";
	   		if(!(ctx->statement(1)->blockstmt())){
           int curnum=++numofblock;
            curFuncName=findfuncname(scopeLevel, curFuncName,curnum);
    	        scopeLevel++;
    	        numofblock=0;
    		visit(ctx->statement(1));   		
    numofblock=curnum;
    scopeLevel=curlevel;
    curFuncName=curfunc;
    	}
    	else{visit(ctx->statement(1));
    scopeLevel=curlevel;
    curFuncName=curfunc;
    }
		}
        return nullptr;
    }

    any visitWhilestmt(BParser::WhilestmtContext *ctx) override {
        cout << "while (";
        visit(ctx->expr());
        cout << ") ";
        string curfunc=curFuncName;
    int curlevel=scopeLevel;
    if(!(ctx->statement()->blockstmt())){
    int curnum=++numofblock;       
            curFuncName=findfuncname(scopeLevel, curFuncName,curnum);
    	        scopeLevel++;
    	        numofblock=0;
    		visit(ctx->statement());
    		
    numofblock=curnum;
    scopeLevel=curlevel;
    curFuncName=curfunc;
    return nullptr;
    }
    
    visit(ctx->statement());
    scopeLevel=curlevel;
    curFuncName=curfunc;
    return nullptr;
    }

    any visitExpressionstmt(BParser::ExpressionstmtContext *ctx) override {
		visit(ctx->expression());
		cout << ";" << endl;
        return nullptr;
    }

    any visitReturnstmt(BParser::ReturnstmtContext *ctx) override {
		cout << "return";
		if (ctx->expression()) {
			cout << " (";
			visit(ctx->expression());
			cout << ")";
		}
		cout << ";" << endl;
        return nullptr;
    }

    any visitNullstmt(BParser::NullstmtContext *ctx) override {
		cout << ";" << endl;
        return nullptr;
    }

    any visitExpr(BParser::ExprContext *ctx) override {
		// unary operator
        if(ctx->atom()) {
            if (ctx->PLUS()) cout << "+";
            else if (ctx->MINUS()) cout << "-";
	    	else if (ctx->NOT()) cout << "!";
	    	visit(ctx->atom()); 
        }
		// binary operator
		else if (ctx->MUL() || ctx->DIV() || ctx->PLUS() || ctx->MINUS() || 
		 		ctx->GT() || ctx->GTE() || ctx->LT() || ctx->LTE() || ctx->EQ() || ctx->NEQ() ||
		 		ctx->AND() || ctx->OR() ) {
	    	visit(ctx->expr(0));
	    	cout << " " << ctx->children[1]->getText() << " "; // print binary operator
	    	visit(ctx->expr(1));
		}
		// ternary operator
		else if (ctx->QUEST()) {
			visit(ctx->expr(0));
			cout << " ? ";
			visit(ctx->expr(1));
			cout << " : ";
			visit(ctx->expr(2));
		}
		else {
			int lineNum = ctx->getStart()->getLine();
			cerr << endl << "[ERROR] visitExpr: unrecognized ops in Line " << lineNum << " --" << ctx->children[1]->getText() << endl;
			exit(-1); // error
        }	
        return nullptr;
    }
   
    any visitAtom(BParser::AtomContext *ctx) override {
		if (ctx->expression()) { // ( expression )
			cout << "("; 
			visit(ctx->expression());
			cout << ")";
		}
		else	// name | constant | funcinvocation
			visit(ctx->children[0]);
        return nullptr;
    }
    
    any visitExpression(BParser::ExpressionContext *ctx) override {
        if (ctx->ASSN()) { // assignment
	   		visit(ctx->name());
	  		 cout << " = ";
		}
		visit(ctx->expr());
        return nullptr;
    }

    any visitFuncinvocation(BParser::FuncinvocationContext *ctx) override {
		cout << ctx->name()->getText() << "(";
		for (int i=0; i < ctx->expr().size(); i++) {
			if (i != 0) cout << ", ";
			visit(ctx->expr(i));
		}
		cout << ")";
        return nullptr;
    }
    
    any visitConstant(BParser::ConstantContext *ctx) override {
        cout << ctx->children[0]->getText();
        return nullptr;
    }
    
    any visitName(BParser::NameContext *ctx) override {
		cout << ctx->NAME()->getText();
        return nullptr;
    }
};

int main(int argc, const char* argv[]) {
    if (argc < 2) {
        cerr << "[Usage] " << argv[0] << "  <input-file>\n";
        exit(0);
    }
    std::ifstream stream;
    stream.open(argv[1]);
    if (stream.fail()) {
        cerr << argv[1] << " : file open fail\n";
        exit(0);
    }

	//cout << "/*-- B2C ANTLR visitor --*/\n";

	ANTLRInputStream inputStream(stream);
	BLexer lexer(&inputStream);
	CommonTokenStream tokenStream(&lexer);
	BParser parser(&tokenStream);
	ParseTree* tree = parser.program();

	// STEP 1. visit parse tree and build symbol tables for functions (PA#1)
	cout << endl << "/*** STEP 1. BUILD SYM_TABS *************" << endl;
	SymbolTableVisitor SymtabTree;
	SymtabTree.visit(tree);
	cout <<         " ---    end of step 1       ------------*/" << endl;

	// STEP 2. visit parse tree and perform type inference for 'auto' typed variables and functions (PA#2)
	cout << endl << "/*** STEP 2. ANALYZE TYPES  *************" << endl;
	TypeAnalysisVisitor AnalyzeTree;
	while(true){
	change=false;
	AnalyzeTree.visit(tree);
BBaseVisitor basevisitor;
	if(!change){break;}}
	for (auto& pair : symTabs){
	pair.second->autoexist();
	}
	AnalyzeTree.visit(tree);
	for (auto& pair : symTabs) {
	    	cout << "--- Symbol Table --- " << pair.first << endl; // function name
	    	pair.second->printSymbols();	
			cout << "";
		}
	cout <<         " ---    end of step 2       ------------*/" << endl;

	// STEP 3. visit parse tree and print out C code with correct types
	cout << endl << "/*** STEP 3. TRANSFORM to C *************/" << endl;
	PrintTreeVisitor PrintTree;
	PrintTree.visit(tree);

	return 0;
}
