#include<bits/stdc++.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/IRBuilder.h>
#include "llvm/IR/Module.h"

using namespace std;
using namespace llvm;

extern map<string, pair<int, int> > vars;
extern map<string, bool> vars_check;

enum var_type
{
	normal=0,array=1
};

class loopInfo 
{
	BasicBlock *afterBB, *checkBB;
	llvm::Value *condition;
	string loopVariable;
	PHINode *phiVariable;
	public:
	loopInfo(BasicBlock *afterBlock, BasicBlock *checkBlock, Value *cond, string var, PHINode *phiVar) {
		afterBB = afterBlock;
		checkBB = checkBlock;
		condition = cond;
		loopVariable = var;
		phiVariable = phiVar;
	}

	BasicBlock *getAfterBlock() { return afterBB; }

	BasicBlock *getCheckBlock() { return checkBB; }

	llvm::Value *getCondition() { return condition; }

	PHINode *getPHINode() { return phiVariable; }

	string getLoopVariable() { return loopVariable; }
};

class Constructs 
{
	public:
		LLVMContext Context;


		Module *TheModule;


		IRBuilder<> *Builder;


		map<string, llvm::AllocaInst *> NamedValues;


		llvm::legacy::FunctionPassManager *TheFPM;

		int errors;

		stack<loopInfo *> *loops;

		Constructs();

		AllocaInst *CreateEntryBlockAlloca(Function *TheFunction, string VarName, string type);

};

class AST;
class Program;
class Field_declarations;
class Field_declaration;
class Var_declarations;
class Var_declaration;
class Method_declarations;
class Method_declaration;
class Method_args_declarations;
class Method_args_declaration;
class Statement;
class Block;
class Field_method_declarations;
class Field_method_declaration;
class Var_method_declarations;
class Var_method_declaration;
class Statements;
class Assignment;
class Function_call;
class If_else;
class Forr;
class Return;
class Break;
class Continue;
class Assign_op;
class Method_call;
class Call_out;
class Call_out_args;
class Call_out_arg;
class Location;
class Exprs;
class Expr;
class Literal;
class Binary_expr;
class Unary_expr;
class Bracket_expr;
class Integer_literal;
class Char_literal;
class Bool_literal;



enum variableType
{
	Array = 1, Normal = 2
};
class Visitor
{
	public:
		virtual int visit(AST *vis){}
		virtual int visit(Program *vis){}
		virtual int visit(Field_declarations *vis){}
		virtual int visit(Field_declaration *vis){}
		// virtual int visit(Var_declarations *vis){}
		virtual int visit(Var_declaration *vis){}
		virtual int visit(Method_declarations *vis){}
		virtual int visit(Method_declaration *vis){}
		virtual int visit(Method_args_declarations *vis){}
		virtual int visit(Method_args_declaration *vis){}
		virtual int visit(Block *vis){}
		virtual int visit(Field_method_declarations *vis){}
		virtual int visit(Field_method_declaration *vis){}
		// virtual int visit(Var_method_declarations *vis){}
		virtual int visit(Var_method_declaration *vis){}
		virtual int visit(Statements *vis){}
		virtual int visit(Statement *vis){}
		virtual int visit(Assignment *vis){}
		virtual int visit(Function_call *vis){}
		virtual int visit(If_else *vis){}
		virtual int visit(Forr *vis){}
		virtual int visit(Return *vis){}
		virtual int visit(Break *vis){}
		virtual int visit(Continue *vis){}
		virtual int visit(Assign_op *vis){}
		virtual int visit(Method_call *vis){}
		virtual int visit(Call_out *vis){}
		virtual int visit(Call_out_args *vis){}
		virtual int visit(Call_out_arg *vis){}
		virtual int visit(Location *vis){}
		virtual int visit(Exprs *vis){}
		virtual int visit(Expr *vis){}
		virtual int visit(Literal *vis){}
		virtual int visit(Binary_expr *vis){}
		virtual int visit(Unary_expr *vis){}
		virtual int visit(Bracket_expr *vis){}
		virtual int visit(Integer_literal *vis){}
		virtual int visit(Char_literal *vis){}
		virtual int visit(Bool_literal *vis){}
};



class AST
{
	public:
		AST();
		virtual int accept(Visitor *v);

};

class Program:public AST
{	public:
	///declarlist *de;
	// statlist *se;
	class Field_declarations *fields;
	class Method_declarations *methods;
	Program(class Field_declarations *, class Method_declarations *);
	virtual Value *generateCode();
	void generateCodeDump();
	Constructs *compilerConstructs;

	//  	Program(class fieldDeclarations *fields, class methodDeclarations *methods) {
	//     this->methods = methods;
	//     this->fields = fields;
	//     // this->compilerConstructs = new Constructs();
	// }
	// Astprogram(){}
	// Astprogram(class statlist *s)
	// {
	// //  this->de=d;
	//  	this->se=s;

	// }
	virtual int accept(Visitor *v){v->visit(this);}
};

class Field_declarations:public AST
{
	public:
		vector<class Field_declaration *> declaration_list;
		Field_declarations() = default;
		void Push_back(class Field_declaration *);
		virtual Value *generateCode(Constructs *compilerConstructs);
		virtual int accept(Visitor *v){v->visit(this);}
};


class Field_declaration:public AST
{
	public:
		string dataType;
		// class Type *typ;
		vector<class Var_declaration *> var_list;
		// class Var_declarations *vars;
		Field_declaration(string, Var_declarations *);
		virtual Value *generateCode(Constructs *compilerConstructs);
		virtual int accept(Visitor *v){v->visit(this);}
};

class Var_declarations:public AST
{
	public:
		vector<class Var_declaration *> var_declaration_list;
		Var_declarations() = default;
		void Push_back(Var_declaration *);
		vector<Var_declaration *> getVarsList();
		virtual int accept(Visitor *v){v->visit(this);}
};

class Var_declaration:public AST
{
	public:		
		variableType declType;
		unsigned int length;
		string name;
		Var_declaration(string, unsigned int);
		explicit Var_declaration(string);
		bool isArray() { return (declType == variableType::Array); }
		unsigned int getLength() {return length;}
		string getname(){return name;}
		virtual int accept(Visitor *v){v->visit(this);}
};	

class Method_declarations:public AST
{
	public:
		vector<class Method_declaration *> method_declaration_list;
		Method_declarations() = default;
		void Push_back(class Method_declaration *);
		virtual Value *generateCode(Constructs *compilerConstructs);
		virtual int accept(Visitor *v){v->visit(this);}
};

class Method_declaration:public AST
{
	public:
		string method_type;
		string name;
		class Method_args_declarations *method_args_declarations;
		class Block *block;
		Method_declaration(string, string, Method_args_declarations *, Block *);
		virtual Function *generateCode(Constructs *compilerConstructs);
		virtual int accept(Visitor *v){v->visit(this);}
};

class Method_args_declarations:public AST
{
	public:
		vector<class Method_args_declaration *> method_args_declaration_list;
		Method_args_declarations() = default;
		void Push_back(class Method_args_declaration *);
		vector<class Method_args_declaration *> getArgList();
		virtual int accept(Visitor *v){v->visit(this);}
};

class Method_args_declaration:public AST
{
	public:
		string arg_type;
		string name;
		Method_args_declaration(string,string);
		string getType(){return arg_type;}
		string getName(){return name;}
		virtual int accept(Visitor *v){v->visit(this);}
};

class Statement:public AST
{
	public:
		Statement() = default;
		virtual bool has_return() { return false; }
		virtual bool has_break() { return false; }
		virtual bool has_continue() { return false; }
		virtual Value *generateCode(Constructs *compilerConstructs){};
		virtual int accept(Visitor *v){v->visit(this);}
};

class Expr:public AST
{
	public:
		// location - 0
		// Function_call - 1
		// literal - 2
		// Binary_expr - 3
		// unary_expr - 4
		// bracket - 5 
		int expr_type=1;
		// int - 0 
		// boolean - 1 
		int lit_type=1;
		int temp = 0;
		Expr() = default;
		virtual int getVal(){return temp;}
		virtual Value *generateCode(Constructs *compilerConstructs) {};
		virtual int accept(Visitor *v){v->visit(this);}
};


class Block:public Statement
{
	public:
		class Field_method_declarations *field_method_declarations;
		class Statements *statements;
		Block(Field_method_declarations *, Statements *);
		bool has_return() ;
		bool has_break() ;
		bool has_continue() ;
		virtual Value *generateCode(Constructs *compilerConstructs);
		virtual int accept(Visitor *v){v->visit(this);}
};

class Field_method_declarations:public AST
{
	public:
		vector<class Field_method_declaration *> declaration_list;
		Field_method_declarations() = default;
		void Push_back(class Field_method_declaration *);
		virtual Value *generateCode(map<string, llvm::AllocaInst *> &, Constructs *);
		virtual int accept(Visitor *v){v->visit(this);} 
};

class Field_method_declaration:public AST
{
	public:
		string dataType;
		vector<Var_method_declaration *> var_list;
		Field_method_declaration(string, Var_method_declarations *);
		virtual Value *generateCode(map<string, llvm::AllocaInst *> &, Constructs *);
		virtual int accept(Visitor *v){v->visit(this);}
};

class Var_method_declarations:public AST
{
	public:
		vector<Var_method_declaration *> var_declaration_list;
		Var_method_declarations() = default;
		void Push_back(Var_method_declaration *);
		vector<Var_method_declaration *> getVarsList();
		virtual int accept(Visitor *v){v->visit(this);}
};

class Var_method_declaration:public AST
{
	public:   
		string name;
		Var_method_declaration(string);
		string getName(){return name;}
		virtual int accept(Visitor *v){v->visit(this);}
};

class Statements:public AST
{
	public:
		vector<Statement *> statement_list;
		Statements() = default;
		void Push_back(Statement *);
		bool has_return();
		bool has_break();
		bool has_continue();
		virtual Value *generateCode(Constructs *compilerConstructs);
		virtual int accept(Visitor *v){v->visit(this);}
};

// class Statement:public AST
// {
//   public:
//     Statement() = default;
//   virtual int accept(Visitor *v){v->visit(this);}
// };

class Assignment:public Statement
{
	public:
		Location *location;
		Assign_op *assign_op;
		Expr *expr;
		Assignment(Location *, Assign_op *, Expr *);
		virtual Value *generateCode(Constructs *compilerConstructs);
		virtual int accept(Visitor *v){v->visit(this);}
};

class Function_call:public Statement, public Expr
{
	public:
		Function_call() = default;
		virtual Value *generateCode(Constructs *compilerConstructs){};
		virtual int accept(Visitor *v){v->visit(this);}
};

class If_else:public Statement
{
	public:
		Expr *expr;
		Block *block1;
		Block *block2;
		If_else(Expr *, Block *, Block *);
		bool has_return();
		bool has_break();
		bool has_continue();
		virtual Value *generateCode(Constructs *compilerConstructs);
		virtual int accept(Visitor *v){v->visit(this);}
};

class Forr:public Statement
{
	public:
		string var_name;
		Expr *expr1;
		Expr *expr2;
		Block *block;
		Forr(string, Expr *, Expr *,Block *);
		virtual Value *generateCode(Constructs *compilerConstructs);
		virtual int accept(Visitor *v){v->visit(this);}
};

class Return:public Statement
{
	public:
		Expr *expr;
		Return(Expr *);
		virtual bool has_return(){return true;}
		virtual Value *generateCode(Constructs *compilerConstructs);
		virtual int accept(Visitor *v){v->visit(this);}
};

class Break:public Statement
{
	public:
		Break() = default;
		virtual bool has_break(){return true;}
		virtual Value *generateCode(Constructs *compilerConstructs);
		virtual int accept(Visitor *v){v->visit(this);}
};

class Continue:public Statement
{
	public:
		Continue() = default;
		virtual bool has_continue(){return true;}
		virtual Value *generateCode(Constructs *compilerConstructs);
		virtual int accept(Visitor *v){v->visit(this);}
};

class Assign_op:public AST
{
	public:
		string operation;
		Assign_op(string);
		virtual int accept(Visitor *v){v->visit(this);}
};

class Method_call:public Function_call
{
	public:
		string name;
		Exprs *exprs;
		Method_call(string, Exprs *);
		virtual Value *generateCode(Constructs *compilerConstructs);  
		virtual int accept(Visitor *v){v->visit(this);}
};

class Call_out:public Function_call
{
	public:
		string print_var;
		Call_out_args *call_out_args;
		Call_out(string, Call_out_args *);
		virtual Value *generateCode(Constructs *compilerConstructs);
		virtual int accept(Visitor *v){v->visit(this);}
};

class Call_out_args:public AST
{
	public:
		vector<Call_out_arg *> Call_out_arg_declaration_list;
		Call_out_args() = default;
		void Push_back(Call_out_arg *);
		virtual int accept(Visitor *v){v->visit(this);}
};

class Call_out_arg:public AST
{
	public:
		Expr *expr;
		string Literal;
		Call_out_arg(class Expr *);
		Call_out_arg(string);
		virtual Value *generateCode(Constructs *compilerConstructs); 
		virtual int accept(Visitor *v){v->visit(this);}
};

class Location:public Expr
{
	public:
		string name;
		Expr *expr;
		var_type location_type;
		explicit Location(string);
		explicit Location(string, Expr *);  
		virtual Value *generateCode(Constructs *compilerConstructs);
		virtual int accept(Visitor *v){v->visit(this);}
};

class Exprs:public AST
{
	public:
		vector<Expr *> Expr_declaration_list;
		Exprs() = default;
		void Push_back(Expr *);  
		virtual int accept(Visitor *v){v->visit(this);}
};

// class Expr:public AST
// {
//   public:
//     Expr() = default;
//   virtual int accept(Visitor *v){v->visit(this);}
// };


class Literal:public Expr
{
	public:
		Literal() = default;
		// int literal_type;
		// this->lit_type = this->literal_type;
		// cout << "hello" << endl;
		// this->expr_type = 2;
		virtual Value *generateCode(Constructs *compilerConstructs){};
		virtual int accept(Visitor *v){v->visit(this);}
};

class Binary_expr:public Expr
{
	public:
		Expr *expr1;
		Expr *expr2;
		string operation;
		Binary_expr(Expr *, string, Expr *);
		virtual Value *generateCode(Constructs *compilerConstructs);
		virtual int accept(Visitor *v){v->visit(this);}
};

class Unary_expr:public Expr
{
	public:
		Expr *expr;
		string operation;
		Unary_expr(string, Expr *);
		virtual Value *generateCode(Constructs *compilerConstructs);
		virtual int accept(Visitor *v){v->visit(this);}
};

class Bracket_expr:public Expr
{
	public:
		Expr *expr;
		// $this->expr_type = 5;
		Bracket_expr(Expr *);
		virtual Value *generateCode(Constructs *compilerConstructs);
		virtual int accept(Visitor *v){v->visit(this);}
};

class Integer_literal:public Literal
{
	public:
		int var;
		Integer_literal(int);
		// this->lit_type = 0;
		virtual int getVal(){return var;}
		virtual Value *generateCode(Constructs *compilerConstructs);
		virtual int accept(Visitor *v){v->visit(this);}
};

class Char_literal:public Literal
{
	public:
		string var;
		Char_literal(string);
		// this->lit_type = 2;
		virtual int accept(Visitor *v){v->visit(this);}
};

class Bool_literal:public Literal
{
	public:
		string var;
		Bool_literal(string);
		// this->lit_type = 1;
		virtual Value *generateCode(Constructs *compilerConstructs);
		virtual int accept(Visitor *v){v->visit(this);}
};
// class Type:public AST
// {
// 	public:
// 		Type(string);
// 	virtual int accept(Visitor *v){v->visit(this);}
// }

// class Int_lit:public AST
// {
// 	public:
// 		Int_lit(int);
// 	virtual int accept(Visitor *v){v->visit(this);}
// }


class DFS:public Visitor
{
	public:
		// vector<statlist *>sa;
		// interpret(vector<statlist *>s){
		//   this->sa=s;
		// }
		int visit(Program *vis)
		{
			cout<<"entered Program\n";
			if(vis->fields!=NULL)
				vis->fields->accept(this);
			if(vis->methods!=NULL)
				vis->methods->accept(this);
		}
		int visit(Field_declarations *vis)
		{
			cout<<"Entered Field_declarations\n";
			for(int i=0;i<vis->declaration_list.size();i++)
				vis->declaration_list[i]->accept(this);
		}
		int visit(Field_declaration *vis)
		{
			cout<<"Entered Field_declaration\n";
			cout<<"Variable Type "<<vis->dataType<<"\n";
			for(int i=0;i<vis->var_list.size();i++)
			{
				vis->var_list[i]->accept(this);
			}
		}
		// int visit(Var_declarations *vis)
		// {

		// }
		int visit(Var_declaration *vis)
		{
			cout<<"Entered Printing Variables\n";
			if(vis->declType==1)
			{
				cout<<"It's an array\n";
				cout<<vis->name<<"\n";
				cout<<vis->length<<"\n";
			}
			else
			{
				cout<<"It's a Normal variable\n";
				cout<<vis->name<<"\n";
			}
		}

		int visit(Method_declarations *vis)
		{
			cout<<"Entered Method Declarations\n";
			for(int i=0;i<vis->method_declaration_list.size();i++)
			{
				vis->method_declaration_list[i]->accept(this);
			}
		}

		int visit(Method_declaration *vis)
		{
			cout<<"Entered Method Declaration\n";
			cout<<"method type "<<vis->method_type<<" method name "<<vis->name<<"\n";
			if(vis->method_args_declarations!=NULL)
			{
				vis->method_args_declarations->accept(this);
			}
			vis->block->accept(this);
		}

		int visit(Method_args_declarations *vis)
		{
			cout<<"Entered Arguments of method\n";
			for(int i=0;i<vis->method_args_declaration_list.size();i++)
			{
				vis->method_args_declaration_list[i]->accept(this);
			}
		}

		int visit(Method_args_declaration *vis)
		{
			cout<<"Argument type "<<vis->arg_type<<" argument name "<<vis->name<<endl;
		}

		int visit(Block *vis)
		{
			cout<<"Enterend Block\n";
			if(vis->field_method_declarations!=NULL)
				vis->field_method_declarations->accept(this);
			if(vis->statements!=NULL)
				vis->statements->accept(this);
		}

		int visit(Field_method_declarations *vis)
		{
			cout<<"Entered Field_method_declarations\n";
			for(int i=0;i<vis->declaration_list.size();i++)
				vis->declaration_list[i]->accept(this);
		}

		int visit(Field_method_declaration *vis)
		{
			cout<<"Entered Field_method_declaration\n";
			cout<<"Variable Type "<<vis->dataType<<"\n";
			for(int i=0;i<vis->var_list.size();i++)
			{
				vis->var_list[i]->accept(this);
			}
		}
		// int visit(Var_declarations *vis)
		// {

		// }
		int visit(Var_method_declaration *vis)
		{
			cout<<"Entered Printing Variables\n";
			cout<<vis->name<<"\n";
		}

		//statements
		int visit(Statements *vis)
		{
			cout<<"Entered Statements\n";
			for(int i=0;i<vis->statement_list.size();i++)
			{
				vis->statement_list[i]->accept(this);
			}
		}

		//statement
		int visit(Statement *vis)
		{
			cout<<"Entered Statement\n";
		}

		//Assignment
		int visit(Assignment *vis)
		{
			cout<<"Entered Assignment\n";
			vis->location->accept(this);
			vis->assign_op->accept(this);
			vis->expr->accept(this);
		}

		//Function_call
		int visit(Function_call *vis)
		{

		}

		//IFelse
		int visit(If_else *vis)
		{
			cout<<"Entered Visit\n";
			vis->expr->accept(this);
			vis->block1->accept(this);
			if(vis->block2!=NULL)
				vis->block2->accept(this);
		}

		//For
		int visit(Forr *vis)
		{
			cout<<"Forr with variable "<<vis->var_name<<endl;
			vis->expr1->accept(this);
			vis->expr2->accept(this);
			vis->block->accept(this);
		}

		//Return
		int visit(Return *vis)
		{
			cout<<"Entered Return\n";
			if(vis->expr!=NULL)
				vis->expr->accept(this);
		}

		//Break
		int visit(Break *vis)
		{
			cout<<"Break Found\n";
		}

		//Continue
		int visit(Continue *vis)
		{
			cout<<"Found Continue\n";
		}

		//Assign_op
		int visit(Assign_op *vis)
		{
			cout<<"Assign_op "<<vis->operation<<endl;
		}

		//Method_call
		int visit(Method_call *vis)
		{
			cout<<"Method call name "<<vis->name<<endl;
			vis->exprs->accept(this);
		}

		//Call_out
		int visit(Call_out *vis)
		{
			cout<<"Call_out Print "<<vis->print_var<<endl;
			vis->call_out_args->accept(this);
		}

		//Call_out_args
		int visit(Call_out_args *vis)
		{
			cout<<"Call out Arguments\n";
			for(int i=0;i<vis->Call_out_arg_declaration_list.size();i++)
			{
				vis->Call_out_arg_declaration_list[i]->accept(this);
			}
		}

		//call_out_arg
		int visit(Call_out_arg *vis)
		{

		}

		//Location
		int visit(Location *vis)
		{
			cout<<"Location variable name "<<vis->name<<endl;
			if(vis->expr!=NULL)
			{
				cout<<"vis->expression is not null\n";
				vis->expr->accept(this);
			}
		}

		//Exprs
		int visit(Exprs *vis)
		{
			cout<<"EXprs\n";
			for(int i=0;i<vis->Expr_declaration_list.size();i++)
			{
				vis->Expr_declaration_list[i]->accept(this);
			}
		}

		//Expr
		int visit(Expr *vis)
		{
			cout<<"Expr\n";
		}

		//Literal
		int visit(Literal *vis)
		{
			cout<<"Literal \n";
		}

		//Binary_expr
		int visit(Binary_expr *vis)
		{
			cout<<"Binary Expr operation "<<vis->operation<<endl;
			vis->expr1->accept(this);
			vis->expr2->accept(this);
		}

		//Unary expr
		int visit(Unary_expr *vis)
		{
			cout<<"Unary_expr operation "<<vis->operation<<endl;
			vis->expr->accept(this);
		}

		//Bravket_expr
		int visit(Bracket_expr *vis)
		{
			cout<<"Bravket_expr\n";
			vis->expr->accept(this);
		}

		//Integer_literal
		int visit(Integer_literal *vis)
		{
			cout<<"Integer literal "<<vis->var<<endl;
		}

		//Char_literal
		int visit(Char_literal *vis)
		{
			cout<<"Char literal "<<vis->var<<endl;
		}

		//Bool_literal
		int visit(Bool_literal *vis)
		{
			cout<<"Bool literal "<<vis->var<<endl;
		}
};
