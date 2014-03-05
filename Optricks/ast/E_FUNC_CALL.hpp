#ifndef E_FUNC_CALL_HPP_
#define E_FUNC_CALL_HPP_
#include "../language/statement/Statement.hpp"
#include "../language/function/AbstractFunction.hpp"
#include "../language/data/Data.hpp"

#define E_FUNC_CALL_C_
//TODO if calling variables are all constants -- inline the function call
//TODO make constructors into generators
class E_FUNC_CALL : public ErrorStatement{
	public:
		Statement* toCall;
		const std::vector<Statement*> vals;
		virtual ~E_FUNC_CALL(){};
		E_FUNC_CALL(PositionID a, Statement* t, const std::vector<Statement*>& val) : ErrorStatement(a),
				toCall(t), vals(val){
		};
		const AbstractClass* getFunctionReturnType(PositionID id, const std::vector<const Evaluatable*>& args)const override final{
			auto type=getReturnType();
			if(type->classType==CLASS_FUNC){
				return ((FunctionClass*)type)->returnType;
			}  else if(type->classType==CLASS_CLASS){
				return type;
			}	else {
				id.error("Class '"+type->getName()+"' cannot be used as function");
				exit(1);
			}
		}
		const Token getToken() const override{
			return T_FUNC_CALL;
		};
		void registerClasses() const override final{
			toCall->registerClasses();
			for(auto &a : vals) a->registerClasses();
		}
		void registerFunctionPrototype(RData& r) const override final{
			toCall->registerFunctionPrototype(r);
			for(auto &a : vals) a->registerFunctionPrototype(r);
		}
		void buildFunction(RData& r) const override final{
			toCall->buildFunction(r);
			for(auto &a : vals) a->buildFunction(r);
		}
		const AbstractClass* getReturnType() const override{
			return toCall->getFunctionReturnType(filePos,*reinterpret_cast<const std::vector<const Evaluatable*>* >(&vals));
		}
		void collectReturns(std::vector<const AbstractClass*>& vals, const AbstractClass* const toBe) override final{}
		const Data* evaluate(RData& a) const override{
			return toCall->evaluate(a)->callFunction(a,filePos,*reinterpret_cast<const std::vector<const Evaluatable*>* >(&vals));
			/*
			if(toCall->getToken()==T_LAMBDAFUNC){
				lambdaFunction* temp = (lambdaFunction*)toCall;
				//TODO be aware that this makes something like
				/ *
				 * for(int i=0; i<7; i+=1) printi((lambda int z: z*i)(i)) // VALID
				 * yet something like
				 * for(int i=0; i<7; i+=1){
				 *  auto tmp = (lambda int z: z*i)
				 *  printi(tmp(i))
				 *  } 													 // INVALID
				 * /
				//for(int i=0; i<7; i+=1){ auto tmp = (lambda int z: z*z) printi(tmp(i)) }
				for(unsigned int i = 0; i<temp->prototype->declarations.size(); i++){
					Declaration* decl = temp->prototype->declarations[i];
					if(i<vals.size()) decl = new Declaration(decl->filePos, decl->classV, decl->variable, false, vals[i]);
					else if(decl->value==NULL || decl->value->getToken()==T_VOID) error("No argument for lambda function!");
					decl->evaluate(a);
				}
				return temp->ret->evaluate(a);
			}
			std::pair<std::vector<Value*>,Data* > tempVal = getArgs(a);
			std::vector<Value*> &Args = tempVal.first;
			Data* d_callee = tempVal.second;
			if(d_callee->getType()==R_GEN){
				//TODO allow for auto tmp = "hello".iterator(), storing "hello" in temp struct
				AbstractClass* cla = d_callee.getFunctionType()->getGeneratorType();
				Value* mine = cla->generateData(a);
				for(unsigned int i = 0; i<Args.size(); i++){
					mine = a.builder.CreateInsertValue(mine, Args[i], ArrayRef<unsigned>(i));
				}
				return new ConstantData(mine, cla);
			}
			Function* callee = d_callee.getMyFunction();
			if(returnType==voidClass){
				a.builder.CreateCall(callee, ArrayRef<Value*>(Args));
				return VOID;
			}
			else{
				Value* t = a.builder.CreateCall(callee, ArrayRef<Value*>(Args), "calltmp");
				return new ConstantData(t, returnType);
			}
			*/
		}
};

#endif /* E_FUNC_CALL_HPP_ */
