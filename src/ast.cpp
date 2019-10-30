#include "ast.h"
#include <bits/stdc++.h>
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Analysis/Interval.h"
#include <llvm/IR/CFG.h>
#include "llvm/IR/BasicBlock.h"
#include <llvm/IR/Function.h>
#include <utility>
#include <llvm/IR/Value.h>

using namespace llvm;
using namespace std;

map<string, pair<int, int> > vars;
map<string, bool> vars_check;


string replace_newline(string str) 
{
	size_t index = 0;
	string search = "\\n";
	while (true) 
	{
		/* Locate the substring to replace. */
		index = str.find(search, index);
		if (index == string::npos) break;
		/* Make the replacement. */
		str.erase(index, search.length());
		str.insert(index, "\n");
		/* Advance index forward so the next iteration doesn't pick it up as well. */
		index += 1;
	}
	return str;
}

Constructs::Constructs() 
{
	this->Builder = new IRBuilder<>(Context);
	this->loops = new stack<loopInfo*>();
	this->TheModule = new Module("Decaf compiler", Context);
	this->TheFPM = new llvm::legacy::FunctionPassManager(TheModule);
	TheFPM->doInitialization();
}

AllocaInst *Constructs::CreateEntryBlockAlloca(Function *TheFunction, string VarName, string type) 
{
	/* Get the builder for current context */
	IRBuilder<> TmpB(&TheFunction->getEntryBlock(), TheFunction->getEntryBlock().begin());
	AllocaInst *alloca_instruction = nullptr;
	if (type == "int") 
	{
		alloca_instruction = TmpB.CreateAlloca(Type::getInt32Ty(this->Context), 0, VarName);
	} 
	else if (type == "boolean") 
	{
		alloca_instruction = TmpB.CreateAlloca(Type::getInt1Ty(this->Context), 0, VarName);
	}
	return alloca_instruction;
}

AST::AST(){}
int  AST::accept(Visitor *v){v->visit(this);}

Program::Program(Field_declarations *field_declarations, Method_declarations *method_declarations)
{
	this->fields = field_declarations;
	this->methods = method_declarations;
	this->compilerConstructs = new Constructs();
}

Value *Program::generateCode()
{
	Value *v;
	if(fields!=NULL)
	{
		// cout<<"Entered Fields\n";
		v=fields->generateCode(this->compilerConstructs);
		if(v==nullptr)
		{
			return nullptr;
		}
	}
	if(methods!=NULL)
	{
		v=methods->generateCode(this->compilerConstructs);

		if(v==nullptr)
		{
			return nullptr;
		}
	}
	return v;
}

raw_ostream &file_write() 
{
	FILE *fp=fopen("outfile","w");
	static raw_fd_ostream S(fileno(fp), true);
	return S;
}
void Program::generateCodeDump()
{
	this->compilerConstructs->TheModule->print(file_write(), nullptr);
}


void Field_declarations::Push_back(Field_declaration *field_declaration)
{
	declaration_list.push_back(field_declaration);
}

Value *Field_declarations::generateCode(Constructs *compilerConstructs)
{
	// cout<<"Entered field_declarations\n";
	for(auto &i:declaration_list)
	{
		// cout<<"Iterating field_declarations\n";
		i->generateCode(compilerConstructs);
	}
	Value *v=ConstantInt::get(compilerConstructs->Context, APInt(32, 1));
	return v;
}




Field_declaration::Field_declaration(string dataType, Var_declarations *var_declarations)
{
	this->dataType = dataType;
	this->var_list = var_declarations->getVarsList();
	for(class Var_declaration *var: this->var_list)
	{
		// cout<<"AAAAAAAAAA addition "<<var->name<<" "<<dataType<<endl;
		if(vars_check.find(var->name) != vars_check.end())
		{
			cout<<"Variable"+ var->name+" is already declared\n";
			exit(0);
		}
		else
		{
			vars_check[var->name] = 1;
			if(dataType=="boolean" and var->declType==2)
			{
				vars[var->name] = make_pair(2,0);
			}
			else if(dataType=="boolean" and var->declType==1)
			{
				vars[var->name] = make_pair(4,var->length);
			}
			else if(dataType=="int" and var->declType==2)
			{
				vars[var->name] = make_pair(1,0); 
			}
			else if(dataType=="int" and var->declType==1)
			{
				vars[var->name] = make_pair(3,var->length); 
			}
		}
	}
	// for (auto it = vars_check.begin(); it != vars_check.end(); ++it  )
	// {
	//    cout << it->first << '\t' << it->second << endl;
	// }
	// for (auto it = vars.begin(); it != vars.end(); ++it  )
	// {
	//    cout << it->first << '\t' << it->second.first << "\t" << it->second.second << endl;
	// }
}

Value *Field_declaration::generateCode(Constructs *compilerConstructs)
{
	// cout<<"Entered Field_declaration\n";
	llvm::Type *ty = nullptr;
	// cout<<"datatype "<<dataType<<endl;
	if(dataType=="int")
	{
		// cout<<"entered Int\n";
		ty = Type::getInt32Ty(compilerConstructs->Context);
		// cout<<"Exited Int\n";
	}
	else if(dataType=="boolean")
	{
		// cout<<"entered boolean\n";
		// cout<<compilerConstructs->Context<<endl;
		ty = Type::getInt1Ty(compilerConstructs->Context);
		// cout<<"Exited boolean\n";
	}
	// cout<<"Done with checking type\n";

	for(auto var: var_list)
	{
		// cout<<"entered var_list loop\n";
		if(var->isArray())
		{
			ArrayType *arrType = ArrayType::get(ty, var->getLength());
			GlobalVariable *gv = new GlobalVariable(*(compilerConstructs->TheModule), arrType, false,
					GlobalValue::ExternalLinkage, nullptr,
					var->getname());
			gv->setInitializer(ConstantAggregateZero::get(arrType));
		}
		else 
		{
			GlobalVariable *gv = new GlobalVariable(*(compilerConstructs->TheModule), ty, false,
					GlobalValue::ExternalLinkage, nullptr,
					var->getname());
			gv->setInitializer(Constant::getNullValue(ty));
		}
	}
	Value *v = ConstantInt::get(compilerConstructs->Context, APInt(32, 1));
	return v;

}



void Var_declarations::Push_back(Var_declaration *var) 
{
	var_declaration_list.push_back(var);
}
vector<Var_declaration *> Var_declarations::getVarsList() 
{
	return var_declaration_list;
}




Var_declaration::Var_declaration(string name, unsigned int array_size) 
{
	this->declType = variableType::Array;
	this->name = name;
	this->length = array_size;
}
Var_declaration::Var_declaration(string name) 
{
	this->declType = variableType::Normal;
	this->name = name;
}



void Method_declarations::Push_back(Method_declaration *method_declaration)
{
	method_declaration_list.push_back(method_declaration);
}

Value *Method_declarations::generateCode(Constructs *compilerConstructs)
{
	// cout<<"Entered Method declarations\n";
	Value *V = ConstantInt::get(compilerConstructs->Context, APInt(32, 0));
	for(auto &i:method_declaration_list)
	{
		V=i->generateCode(compilerConstructs);
		if(V==nullptr)
			return V;
	}
	return V;
}


Method_declaration::Method_declaration(string method_type, string name, Method_args_declarations *method_args_declarations, Block *block)
{
	this->method_type = method_type;
	this->name = name;
	this->method_args_declarations = method_args_declarations;
	this->block = block;
	if(method_args_declarations!=NULL)
	{
		for(class Method_args_declaration *var: method_args_declarations->method_args_declaration_list)
		{
			// cout<<"AAAAAAAAAA deletion "<<var->name<<" "<<var->arg_type<<endl;
			vars_check.erase(var->name);
			vars.erase(var->name);
			// vars_check[var->name] = 0;
		}
	}
	// for (auto it = vars_check.begin(); it != vars_check.end(); ++it  )
	// {
	//    cout << it->first << '\t' << it->second << endl;
	// }
	// for (auto it = vars.begin(); it != vars.end(); ++it  )
	// {
	//    cout << it->first << '\t' << it->second.first << "\t" << it->second.second << endl;
	// }

}

Function* Method_declaration::generateCode(Constructs *compilerConstructs)
{
	// cout<<"Entered Method declaration\n";
	vector<string> argNames;
	vector<string> argTypes;
	vector<class Method_args_declaration *> args;
	if(method_args_declarations!=NULL)
	{
		args = method_args_declarations->getArgList();
	}

	vector<Type *> arguments;
	auto arg_size = args.size();
	for (auto &arg : args) {
		string arg_type = arg->getType();
		string arg_name = arg->getName();
		if (arg_type == "int") 
		{
			arguments.push_back(Type::getInt32Ty(compilerConstructs->Context));
		} else if (arg_type == "boolean") 
		{
			arguments.push_back(Type::getInt1Ty(compilerConstructs->Context));
		} 
		argTypes.emplace_back(arg_type);
		argNames.emplace_back(arg_name);
	}

	Type *returnType;

	if (method_type == "int") 
	{
		returnType = Type::getInt32Ty(compilerConstructs->Context);
	} 
	else if (method_type == "boolean") 
	{
		returnType = Type::getInt1Ty(compilerConstructs->Context);
	} 
	else if (method_type == "void") 
	{
		returnType = Type::getVoidTy(compilerConstructs->Context);
	} 
	else 
	{
		cerr<<"Invalid Return Type for " + name + ". Return Type can only be int or boolean or bool"<<endl;
		return nullptr;
	}

	FunctionType *FT = llvm::FunctionType::get(returnType, arguments, false);
	Function *F = llvm::Function::Create(FT, Function::ExternalLinkage, name, compilerConstructs->TheModule);

	unsigned Idx = 0;
	for (Function::arg_iterator AI = F->arg_begin(); Idx != arg_size; ++AI, ++Idx) 
	{
		AI->setName(argNames[Idx]);
	}

	BasicBlock *BB = BasicBlock::Create(compilerConstructs->Context, "entry", F);
	compilerConstructs->Builder->SetInsertPoint(BB);
	Idx = 0;

	for (auto &Arg : F->args()) 
	{
		AllocaInst *Alloca = compilerConstructs->CreateEntryBlockAlloca(F, argNames[Idx], argTypes[Idx]);
		compilerConstructs->Builder->CreateStore(&Arg, Alloca);
		compilerConstructs->NamedValues[argNames[Idx]] = Alloca;
		Idx++;
	}

	Value *RetVal = block->generateCode(compilerConstructs);
	if (RetVal) 
	{
		if (method_type == "void")
			compilerConstructs->Builder->CreateRetVoid();
		verifyFunction(*F);
		compilerConstructs->TheFPM->run(*F);
		return F;
	}

	F->eraseFromParent();
	return nullptr;


}




void Method_args_declarations::Push_back(Method_args_declaration *method_args_declaration)
{
	this->method_args_declaration_list.push_back(method_args_declaration);
}

vector<class Method_args_declaration *> Method_args_declarations::getArgList() 
{
	return method_args_declaration_list;
}




Method_args_declaration::Method_args_declaration(string arg_type, string name)
{
	this->arg_type = arg_type;
	this->name = name;
	// cout<<"AAAAAAAAAA addition "<<name<<" "<<arg_type<<endl;
	if(vars_check.find(name) != vars_check.end())
	{
		cout<<"Variable"+ name+" is already declared\n";
		exit(0);
	}
	else
	{
		vars_check[name] = 1;
		if(arg_type=="boolean")
		{
			vars[name] = make_pair(2,0);
		}
		else if(arg_type=="int")
		{
			vars[name] = make_pair(1,0); 
		}
	}
	// for (auto it = vars_check.begin(); it != vars_check.end(); ++it  )
	// {
	//    cout << it->first << '\t' << it->second << endl;
	// }
	// for (auto it = vars.begin(); it != vars.end(); ++it  )
	// {
	//    cout << it->first << '\t' << it->second.first << "\t" << it->second.second << endl;
	// }
}



Block::Block(Field_method_declarations *field_method_declarations, Statements *statements)
{
	this->field_method_declarations = field_method_declarations;
	this->statements = statements;
	if(field_method_declarations!=NULL)
	{
		for(class Field_method_declaration *var: field_method_declarations->declaration_list)
		{
			for(class Var_method_declaration *varr: var->var_list)
			{
				vars_check.erase(varr->name);
				vars.erase(varr->name);
			}
		}
	}
	// for (auto it = vars_check.begin(); it != vars_check.end(); ++it  )
	// {
	//    cout << it->first << '\t' << it->second << endl;
	// }
	// for (auto it = vars.begin(); it != vars.end(); ++it  )
	// {
	//    cout << it->first << '\t' << it->second.first << "\t" << it->second.second << endl;
	// }
}

bool Block::has_return()
{
	return statements->has_return();
}
bool Block::has_break()
{
	return statements->has_break();
}
bool Block::has_continue()
{
	return statements->has_continue();
}

Value* Block::generateCode(Constructs *compilerConstructs)
{
	// cout<<"Block\n";
	Value *V;
	map<string, llvm::AllocaInst *> Old_vals;
	if(field_method_declarations!=NULL)
	{
		V = field_method_declarations->generateCode(Old_vals, compilerConstructs);
		if (V == nullptr) 
		{
			return V;
		}
	}
	if(statements!=NULL)
	{
		V=statements->generateCode(compilerConstructs);
		for (auto it = Old_vals.begin(); it != Old_vals.end(); it++) {
			compilerConstructs->NamedValues[it->first] = Old_vals[it->first];
		}
	}
	return V;


}



void Field_method_declarations::Push_back(Field_method_declaration *field_method_declaration)
{
	declaration_list.push_back(field_method_declaration);
}

Value* Field_method_declarations::generateCode(map<string, AllocaInst *> &oldValues, Constructs *compilerConstructs)
{
	// cout<<"Field_method_declarations\n";
	Value *v = ConstantInt::get(compilerConstructs->Context, APInt(32, 1));
	for (auto &decl : declaration_list) 
	{
		v = decl->generateCode(oldValues, compilerConstructs);
		if (v == nullptr) 
		{
			return v;
		}
	}
	return v;
}



Field_method_declaration::Field_method_declaration(string dataType, Var_method_declarations *var_method_declarations)
{
	this->dataType = dataType;
	this->var_list = var_method_declarations->getVarsList();
	for(class Var_method_declaration *var: this->var_list)
	{
		// cout<<"AAAAAAAAAA inside method addition "<<var->name<<" "<<dataType<<endl;
		if(vars_check.find(var->name) != vars_check.end())
		{
			cout<<"Variable"+ var->name+" is already declared\n";
			exit(0);
		}
		else
		{
			vars_check[var->name] = 1;
			if(dataType=="boolean")
			{
				vars[var->name] = make_pair(2,0);
			}
			else if(dataType=="int")
			{
				vars[var->name] = make_pair(1,0); 
			}
		}
	}
	// for (auto it = vars_check.begin(); it != vars_check.end(); ++it  )
	// {
	//    cout << it->first << '\t' << it->second << endl;
	// }
	// for (auto it = vars.begin(); it != vars.end(); ++it  )
	// {
	//    cout << it->first << '\t' << it->second.first << "\t" << it->second.second << endl;
	// }
}

Value* Field_method_declaration::generateCode(map<string, llvm::AllocaInst *> &Old_vals, Constructs *compilerConstructs)
{
	// cout<<"Field_method_declaration\n";
	llvm::Function *TheFunction = compilerConstructs->Builder->GetInsertBlock()->getParent();
	for (const auto &var : var_list) 
	{
		string varName = var->getName();
		llvm::Value *initval = nullptr;
		llvm::AllocaInst *Alloca = nullptr;
		if (dataType == "int") 
		{
			initval = ConstantInt::get(compilerConstructs->Context, APInt(32, 0));
			Alloca = compilerConstructs->CreateEntryBlockAlloca(TheFunction, varName, "int");
		} 
		else if (dataType == "boolean") 
		{
			initval = ConstantInt::get(compilerConstructs->Context, APInt(1, 0));
			Alloca = compilerConstructs->CreateEntryBlockAlloca(TheFunction, varName, "boolean");
		}
		compilerConstructs->Builder->CreateStore(initval, Alloca);

		Old_vals[varName] = compilerConstructs->NamedValues[varName];
		compilerConstructs->NamedValues[varName] = Alloca;
	}
	Value *v = ConstantInt::get(compilerConstructs->Context, APInt(32, 1));
	return v;
}



void Var_method_declarations::Push_back(Var_method_declaration *var_method_declaration) 
{
	var_declaration_list.push_back(var_method_declaration);
}
vector<Var_method_declaration *> Var_method_declarations::getVarsList() 
{
	return var_declaration_list;
}




Var_method_declaration::Var_method_declaration(string name) 
{
	this->name = name;
}


// Statements
void Statements::Push_back(Statement * statement)
{
	this->statement_list.push_back(statement);
}

bool Statements::has_return() 
{
	for (int i = 0; i < statement_list.size(); i++) 
	{
		if (statement_list[i]->has_return()) 
		{
			return true;
		}
	}
	return false;
}

bool Statements::has_continue() 
{
	for (int i = 0; i < statement_list.size(); i++) 
	{
		if (statement_list[i]->has_continue()) 
		{
			return true;
		}
	}
	return false;
}

bool Statements::has_break() 
{
	for (int i = 0; i < statement_list.size(); i++) 
	{
		if (statement_list[i]->has_break()) 
		{
			return true;
		}
	}
	return false;
}


Value *Statements::generateCode(Constructs* compilerConstructs)
{
	// cout<<"Entered Statements\n";
	Value *v = ConstantInt::get(compilerConstructs->Context, llvm::APInt(32, 1));
	for (auto &stmt : statement_list) 
	{
		v = stmt->generateCode(compilerConstructs);
	}
	return v;
}

// Statement

// Assignment
Assignment::Assignment(Location *location, Assign_op *assign_op, Expr *expr)
{
	this->location = location;
	this->assign_op = assign_op;
	this->expr = expr;
	if(location->lit_type != expr->lit_type)
	{
		cout<<"Assignment Literals should be same on both sides\n";
		exit(0);
	}
}

Value* Assignment::generateCode(Constructs *compilerConstructs)
{
	// cout<<"Entered Assignment\n";
	Value *cur = compilerConstructs->NamedValues[location->name];

	if (cur == nullptr) 
	{
		cur = compilerConstructs->TheModule->getGlobalVariable(location->name);
	}

	Value *val = expr->generateCode(compilerConstructs);
	if (expr->expr_type==0) 
	{
		val = compilerConstructs->Builder->CreateLoad(val);
	}

	Value *lhs = location->generateCode(compilerConstructs);
	cur = compilerConstructs->Builder->CreateLoad(lhs);


	if (assign_op->operation == "+=") 
	{
		val = compilerConstructs->Builder->CreateAdd(cur, val, "addEqualToTmp");
	} 
	else if (assign_op->operation == "-=") 
	{
		val = compilerConstructs->Builder->CreateSub(cur, val, "subEqualToTmp");
	}
	return compilerConstructs->Builder->CreateStore(val, lhs);


}


//Function_call


//If_else
If_else::If_else(Expr *expr, Block *block1, Block *block2)
{
	this->expr = expr;
	if(expr->lit_type!=1)
	{
		cout<<"If condition should be boolen"<<endl;
		exit(0);
	}
	this->block1 = block1;
	this->block2 = block2;
}

bool If_else::has_return() 
{
	bool status = false;
	if (block1 != nullptr) 
	{
		status = status | block1->has_return();
	}
	if (block2 != nullptr) {
		status = status | block2->has_return();
	}
	return status;
}


bool If_else::has_continue() 
{
	bool status = false;
	if (block1 != nullptr) 
	{
		status = status | block1->has_continue();
	}
	if (block2 != nullptr) 
	{
		status = status | block2->has_continue();
	}
	return status;
}


bool If_else::has_break() 
{
	bool status = false;
	if (block1 != nullptr) 
	{
		status = status | block1->has_break();
	}
	if (block2 != nullptr) 
	{
		status = status | block2->has_break();
	}
	return status;
}

Value* If_else::generateCode(Constructs *compilerConstructs)
{
	// cout<<"Entered If_else\n";
	Value *cond = expr->generateCode(compilerConstructs);

	Function *TheFunction = compilerConstructs->Builder->GetInsertBlock()->getParent();
	BasicBlock *ifBlock = BasicBlock::Create(compilerConstructs->Context, "if", TheFunction);
	BasicBlock *elseBlock = BasicBlock::Create(compilerConstructs->Context, "else");
	BasicBlock *nextBlock = BasicBlock::Create(compilerConstructs->Context, "ifcont");
	BasicBlock *otherBlock = elseBlock;
	bool ret_if = block1->has_return(), ret_else = false;
	bool break_if=block1->has_break();
	bool continue_if=block1->has_continue();

	if (block2 == nullptr) 
	{
		cout<<"Block2 is null pointer"<<endl;
		otherBlock = nextBlock;
	}

	compilerConstructs->Builder->CreateCondBr(cond, ifBlock, otherBlock);
	compilerConstructs->Builder->SetInsertPoint(ifBlock);

	Value *if_val = block1->generateCode(compilerConstructs);
	if (if_val == nullptr) 
	{
		return nullptr;
	}

	bool flag = 0;
	if (!ret_if && !break_if && !continue_if) 
	{
		compilerConstructs->Builder->CreateBr(nextBlock);
		flag = 1;
	}

	ifBlock = compilerConstructs->Builder->GetInsertBlock();

	Value *else_val = nullptr;

	if (block2 != nullptr) 
	{
		TheFunction->getBasicBlockList().push_back(elseBlock);
		compilerConstructs->Builder->SetInsertPoint(elseBlock);
		else_val = block2->generateCode(compilerConstructs);
		if (else_val == nullptr) 
		{
			return nullptr;
		}
		ret_else = block2->has_return();
		if (!ret_else)
			compilerConstructs->Builder->CreateBr(nextBlock);
	}

	TheFunction->getBasicBlockList().push_back(nextBlock);
	compilerConstructs->Builder->SetInsertPoint(nextBlock);
	if (ret_else && ret_if) 
	{
		Type *retType = compilerConstructs->Builder->GetInsertBlock()->getParent()->getReturnType();
		if (retType == Type::getVoidTy(compilerConstructs->Context))
			compilerConstructs->Builder->CreateRetVoid();
		else 
		{
			compilerConstructs->Builder->CreateRet(ConstantInt::get(compilerConstructs->Context, APInt(32, 0)));
		}
	}
	Value *V = ConstantInt::get(compilerConstructs->Context, APInt(32, 0));
	return V;
}


//For
Forr::Forr(string var_name, Expr *expr1, Expr *expr2, Block *block)
{
	this->var_name = var_name;
	this->expr1 = expr1;
	this->expr2 = expr2;
	this->block = block;
}

Value* Forr::generateCode(Constructs *compilerConstructs)
{
	// cout<<"Entered For\n";
	Value *start = expr1->generateCode(compilerConstructs);
	if (start == nullptr) 
	{
		return nullptr;
	}
	if (expr1->expr_type==0) 
	{
		start = compilerConstructs->Builder->CreateLoad(start);
	}
	/* Get the parent method of this for loop */
	Function *TheFunction = compilerConstructs->Builder->GetInsertBlock()->getParent();
	/* Create memory for the loop variable */
	llvm::AllocaInst *Alloca = compilerConstructs->CreateEntryBlockAlloca(TheFunction, var_name, string("int"));
	compilerConstructs->Builder->CreateStore(start, Alloca);

	Value *step_val = ConstantInt::get(compilerConstructs->Context, APInt(32, 1));
	BasicBlock *pre_header_basic_block = compilerConstructs->Builder->GetInsertBlock();
	BasicBlock *loop_body = BasicBlock::Create(compilerConstructs->Context, "loop", TheFunction);
	BasicBlock *afterBB = BasicBlock::Create(compilerConstructs->Context, "afterloop", TheFunction);
	compilerConstructs->Builder->CreateBr(loop_body);
	compilerConstructs->Builder->SetInsertPoint(loop_body);

	PHINode *Variable = compilerConstructs->Builder->CreatePHI(Type::getInt32Ty(compilerConstructs->Context), 2, var_name);
	Variable->addIncoming(start, pre_header_basic_block);
	/* Store the old value */
	Value *cond = expr2->generateCode(compilerConstructs);

	// Check if condition is a location
	if (expr2->expr_type==0) 
	{
		cond = compilerConstructs->Builder->CreateLoad(cond);
	}
	compilerConstructs->loops->push(new loopInfo(afterBB, loop_body, cond, var_name, Variable));
	llvm::AllocaInst *OldVal = compilerConstructs->NamedValues[var_name];
	compilerConstructs->NamedValues[var_name] = Alloca;
	/* Generate the code for the body */
	if (block->generateCode(compilerConstructs) == nullptr) 
	{
		return nullptr;
	}

	Value *cur = compilerConstructs->Builder->CreateLoad(Alloca, var_name);
	Value *next_val = compilerConstructs->Builder->CreateAdd(cur, step_val, "NextVal");
	compilerConstructs->Builder->CreateStore(next_val, Alloca);
	cond = compilerConstructs->Builder->CreateICmpSLT(next_val, cond, "loopcondition");
	BasicBlock *loopEndBlock = compilerConstructs->Builder->GetInsertBlock();
	compilerConstructs->Builder->CreateCondBr(cond, loop_body, afterBB);
	compilerConstructs->Builder->SetInsertPoint(afterBB);
	Variable->addIncoming(next_val, loopEndBlock);

	if (OldVal) 
	{
		compilerConstructs->NamedValues[var_name] = OldVal;
	} 
	else 
	{
		compilerConstructs->NamedValues.erase(var_name);
	}
	llvm::Value *V = ConstantInt::get(compilerConstructs->Context, APInt(32, 1));
	return V;
}

//Return
Return::Return(Expr *expr)
{
	this->expr = expr;
}

Value *Return::generateCode(Constructs *compilerConstructs)
{
	// cout<<"Entered Return\n";
	llvm::Value *V = nullptr;
	if (expr != nullptr) 
	{
		/// Generate IR for expression to be returned
		V = expr->generateCode(compilerConstructs);
		if (expr->expr_type==0) 
		{
			/// Generate IR for returning it
			V = compilerConstructs->Builder->CreateLoad(V);
		}
		compilerConstructs->Builder->CreateRet(V);
		return V;
	}
	compilerConstructs->Builder->CreateRetVoid();
	return V;
}

//Break

Value *Break::generateCode(Constructs *compilerConstructs) 
{
	// cout<<"Entered Break\n";
	llvm::Value *V = llvm::ConstantInt::get(compilerConstructs->Context, llvm::APInt(32, 1));
	loopInfo *currentLoop = compilerConstructs->loops->top();
	compilerConstructs->Builder->CreateBr(currentLoop->getAfterBlock());
	return V;
}

//Continue
Value *Continue::generateCode(Constructs *compilerConstructs) 
{
	// cout<<"Entered Continue\n";
	llvm::Value *V = llvm::ConstantInt::get(compilerConstructs->Context, llvm::APInt(32, 1));
	loopInfo *currentLoop = compilerConstructs->loops->top();
	Expr *condition = nullptr;
	string var = currentLoop->getLoopVariable();
	AllocaInst *Alloca = compilerConstructs->NamedValues[var];
	Value *step_val = ConstantInt::get(compilerConstructs->Context, APInt(32, 1));
	Value *cur = compilerConstructs->Builder->CreateLoad(Alloca, var);
	Value *next_val = compilerConstructs->Builder->CreateAdd(cur, step_val, "NextVal");
	compilerConstructs->Builder->CreateStore(next_val, Alloca);
	llvm::Value *cond = compilerConstructs->Builder->CreateICmpULE(next_val, currentLoop->getCondition(),
			"loopcondition");
	BasicBlock *loopEndBlock = compilerConstructs->Builder->GetInsertBlock();
	compilerConstructs->Builder->CreateCondBr(cond, currentLoop->getCheckBlock(), currentLoop->getAfterBlock());
	return V;
}

//Assign_op
Assign_op::Assign_op(string operation)
{
	this->operation = operation;
}

// Method_call
Method_call::Method_call(string name, Exprs *exprs)
{
	this->name = name;
	this->exprs = exprs;
	this->expr_type = 1;
	this->lit_type = 0;
}

Value* Method_call::generateCode(Constructs *compilerConstructs)
{
	// cout<<"Entered Method_call\n";
	Function *calle = compilerConstructs->TheModule->getFunction(name);
	vector<class Expr*> args_list;
	if( exprs!=NULL)
		args_list = exprs->Expr_declaration_list;

	vector<Value *> Args;
	for (auto &arg : args_list) 
	{
		Value *argVal = arg->generateCode(compilerConstructs);
		if (arg->expr_type == 0) 
		{
			argVal = compilerConstructs->Builder->CreateLoad(argVal);
		}
		if (argVal == nullptr) 
		{
			return nullptr;
		}
		Args.push_back(argVal);
	}
	Value *v = compilerConstructs->Builder->CreateCall(calle, Args);
	return v;
}

//Call_out
Call_out::Call_out(string print_var, Call_out_args *call_out_args)
{
	this->print_var=print_var.substr(1, print_var.length() - 2);
	this->print_var=replace_newline(this->print_var);
	// this->print_var = print_var;
	this->call_out_args = call_out_args;
}

Value* Call_out::generateCode(Constructs *compilerConstructs)
{
	// cout<<"Entered Call_out "<<print_var<<endl;
	vector<llvm::Type *> argTypes;
	vector<Value *> Args;
	vector<class Call_out_arg *> args_list;
	if(call_out_args!=NULL) 
		args_list= call_out_args->Call_out_arg_declaration_list;


	for (auto &i : args_list) 
	{
		Value *tmp = i->generateCode(compilerConstructs);
		if (tmp == nullptr) 
		{
			return nullptr;
		}
		Args.push_back(tmp);
		argTypes.push_back(tmp->getType());
	}

	llvm::ArrayRef<llvm::Type *> argsRef(argTypes);
	llvm::ArrayRef<llvm::Value *> funcargs(Args);
	llvm::FunctionType *FType = FunctionType::get(Type::getInt32Ty(compilerConstructs->Context), argsRef, false);
	Constant *func = compilerConstructs->TheModule->getOrInsertFunction(print_var, FType);
	Value *v = compilerConstructs->Builder->CreateCall(func, funcargs);
	return v;
}

// Call_out_args
void Call_out_args::Push_back(Call_out_arg *call_out_arg)
{
	Call_out_arg_declaration_list.push_back(call_out_arg);
}


// Call_out_arg
Call_out_arg::Call_out_arg(class Expr *expr)
{
	this->expr = expr; 
}
Call_out_arg::Call_out_arg(string Literal)
{
	this->Literal=Literal.substr(1, Literal.length() - 2);
	this->Literal=replace_newline(this->Literal);
	this->expr = NULL;
}

Value *Call_out_arg::generateCode(Constructs *compilerConstructs)
{
	// cout<<"Entered Call_out_arg "<<Literal<<endl;
	Value *v;
	if(expr != nullptr)
	{
		v = expr->generateCode(compilerConstructs);
		if (expr->expr_type==0) 
		{
			v = compilerConstructs->Builder->CreateLoad(v);
		}
	}
	else
	{
		v = compilerConstructs->Builder->CreateGlobalStringPtr(Literal);
	}
	// cout<<"Exited from Call_out_arg\n";
	return v;
}

//Location
Location::Location(string name)
{
	this->name = name;
	this->expr = NULL;
	this->expr_type = 0; 
	if(vars_check.find(name) == vars_check.end())
	{
		cout<<"Variable "<<name<<" not found\n";
		exit(0);
	}
	else
	{
		if(vars[name].first==3 || vars[name].first==4)
		{
			cout<<"Its not an array variable\n";
			exit(0);
		}
		else
		{
			this->lit_type = vars[name].first-1;
		}
	}
	this->location_type=var_type::normal;
	// cout<<"Entered Location "<<name<<" "<<this->lit_type<<endl;
}
Location::Location(string name, Expr *expr)
{
	this->name = name;
	this->expr = expr;
	this->expr_type=0;
	this->location_type=var_type::array;
	if(vars_check.find(name) == vars_check.end())
	{
		cout<<"Variable "<<name<<" not found\n";
		exit(0);
	}
	else
	{
		if(vars[name].first==1 || vars[name].first==2)
		{
			cout<<"Its not an array variable\n";
			exit(0);
		}
		else
		{
			if(expr->expr_type!=2)
			{
				// cout<<"should be int literal\n";
				// exit(0);
			}
			else if(expr->getVal() >= vars[name].second and expr->getVal() < 0)
			{
				cout<<"Over bounds\n";
				exit(0);
			}
			this->lit_type = 0;
		}
	}
}

Value *Location::generateCode(Constructs *compilerConstructs)
{
	// cout<<"Entered Location "<<name<<endl;
	Value *V = compilerConstructs->NamedValues[name];
	if (V == nullptr) 
	{
		V = compilerConstructs->TheModule->getNamedGlobal(name);
	}

	if (this->location_type == var_type::normal) {
		return V;
	}


	Value *index = expr->generateCode(compilerConstructs);
	if (expr->expr_type == 0) 
	{
		index = compilerConstructs->Builder->CreateLoad(index);
	}



	vector<Value *> array_index;
	array_index.push_back(compilerConstructs->Builder->getInt32(0));
	array_index.push_back(index);
	V = compilerConstructs->Builder->CreateGEP(V, array_index, name + "_Index");
	return V;

}


//Exprs
void Exprs::Push_back(Expr *expr)
{
	this->Expr_declaration_list.push_back(expr);
}

// Expr

//Literal

// Binary_expr
Binary_expr::Binary_expr(Expr *expr1, string operation, Expr *expr2)
{
	this->expr1 = expr1;
	this->operation = operation;
	this->expr2 = expr2;
	this->expr_type = 3;
	// cout<<"Binary "<<expr1->lit_type<<" "<<expr2->lit_type<<" "<<operation<<endl;
	string a = operation;
	if(((a=="+" or a=="-") or (a=="*" or (a=="/" or a=="%"))))
	{
		if(!(expr1->lit_type==0 && expr2->lit_type==0))
		{
			cout<<"Arithemetic expression should have type ints on two sides\n";
			exit(0);
		}
		// cout<<"Arithemetic Expression "<<operation<<endl;
		this->lit_type = 0;
	}
	else if(((a=="<" or a==">") or (a=="<=" or a==">=")))
	{
		if(!(expr1->lit_type==0 && expr2->lit_type==0))
		{
			cout<<"Relation expression should have type ints on two sides\n";
			exit(0);
		}
		// cout<<"Relation operation\n";
		this->lit_type = 1;
	}
	// cout<<"Binary expression "<<this->lit_type<<endl;
}

Value *Binary_expr::generateCode(Constructs *compilerConstructs)
{
	// cout<<"Entered Binay_expr "<<operation<<endl;
	Value *left = expr1->generateCode(compilerConstructs);
	Value *right = expr2->generateCode(compilerConstructs);
	if (expr1->expr_type == 0) 
	{
		left = compilerConstructs->Builder->CreateLoad(left);
	}
	if (expr2->expr_type == 0) 
	{
		right = compilerConstructs->Builder->CreateLoad(right);
	}
	string opr=operation;

	Value *v = nullptr;
	if (operation == "+") 
	{
		v = compilerConstructs->Builder->CreateAdd(left, right, "addtmp");
	} 
	else if (operation == "-") 
	{
		v = compilerConstructs->Builder->CreateSub(left, right, "subtmp");
	} 
	else if (operation == "*") 
	{
		v = compilerConstructs->Builder->CreateMul(left, right, "multmp");
	} 
	else if (operation == "/") 
	{
		v = compilerConstructs->Builder->CreateSDiv(left, right, "divtmp");
	} 
	else if (operation == "%") 
	{
		v = compilerConstructs->Builder->CreateSRem(left, right, "modtmp");
	} 
	else if (operation == "<") 
	{
		v = compilerConstructs->Builder->CreateICmpSLT(left, right, "ltcomparetmp");
	} 
	else if (operation == ">") 
	{
		v = compilerConstructs->Builder->CreateICmpSGT(left, right, "gtcomparetmp");
	} 
	else if (operation == "<=") 
	{
		v = compilerConstructs->Builder->CreateICmpSLE(left, right, "lecomparetmp");
	} 
	else if (operation == ">=") 
	{
		v = compilerConstructs->Builder->CreateICmpSGE(left, right, "gecomparetmp");
	}
	else if (operation == "==") 
	{
		v = compilerConstructs->Builder->CreateICmpEQ(left, right, "equalcomparetmp");
	}
	else if (operation == "!=") 
	{
		v = compilerConstructs->Builder->CreateICmpNE(left, right, "notequalcomparetmp");
	}
	else if (opr=="&&")
	{
		v = compilerConstructs->Builder->CreateAnd(left, right, "andtmp");
	}
	else if (opr=="||")
	{
		v = compilerConstructs->Builder->CreateOr(left, right, "ortmp");
	}
	return v;
}

//Unary_expr
Unary_expr::Unary_expr(string operation, Expr *expr)
{
	this->expr = expr;
	this->operation = operation;
	this->expr_type = 4;
	// cout<<"unary expression "<<operation<<endl;
	if(operation== "!" and expr->lit_type!=1)
	{
		cout<<"Unary expression variable should be boolean for !\n";
		exit(0);
	}
	else if(operation=="!")
		this->lit_type = 1;
	if(operation== "-" and expr->lit_type!=0)
	{
		cout<<"Unary expression variable should be int for -\n";
		exit(0);
	}
	else if(operation=="-")
		this->lit_type = 0;
	// cout<<"unary_lit_type "<<this->lit_type<<endl;
}

Value *Unary_expr::generateCode(Constructs *compilerConstructs)
{
	// cout<<"Entered Unary_expr\n";
	Value *v = expr->generateCode(compilerConstructs);
	if (expr->expr_type==0) {
		v = compilerConstructs->Builder->CreateLoad(v);
	}

	if (operation == "-") {
		return compilerConstructs->Builder->CreateNeg(v, "negtmp");
	} else if (operation == "!") {
		return compilerConstructs->Builder->CreateNot(v, "nottmp");
	}
}

//Bracket_expr
Bracket_expr::Bracket_expr(Expr *expr)
{
	this->expr = expr;
	this->expr_type = 5;
	this->lit_type = expr->lit_type;
}

Value *Bracket_expr::generateCode(Constructs *compilerConstructs)
{
	// cout<<"Bracket_expr\n";
	return expr->generateCode(compilerConstructs);
}

//Integer_literal
Integer_literal::Integer_literal(int var)
{
	this->var = var;
	this->lit_type = 0;
	this->expr_type = 2;
	// cout<<"Int_literal "<<this->lit_type<<endl;
}

Value* Integer_literal::generateCode(Constructs *compilerConstructs)
{
	// cout<<"Entered Integer_lit\n";
	Value *v = ConstantInt::get(compilerConstructs->Context, llvm::APInt(32, static_cast<uint64_t>(var)));
	return v;
}

//Char_literal
Char_literal::Char_literal(string var)
{
	this->var = var;
	this->lit_type = 2;
	this->expr_type = 2;
	// cout<<"Bool_literal "<<this->lit_type<<endl;
}

//Bool_literal
Bool_literal::Bool_literal(string var)
{
	this->var = var;
	this->lit_type = 1;
	this->expr_type = 2;
	// cout<<"Bool_literal "<<this->lit_type<<endl;
}

Value* Bool_literal::generateCode(Constructs *compilerConstructs)
{
	// cout<<"Entered Bool_lit\n";
	bool val;
	if (var == "true") val = true;
	else if (var == "false") val = false;
	Value *v = ConstantInt::get(compilerConstructs->Context, llvm::APInt(1, val));
	return v;
}
