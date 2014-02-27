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
		const AbstractClass* getFunctionReturnType(PositionID id, const std::vector<Evaluatable*>& args)const{
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
		/*inline FunctionProto* generateFunctionProto() const{
			std::vector<Declaration*> dec;
			for(auto& a:vals){
				AbstractClass* cp=a->getReturnType();
				if(cp==NULL) error("TYPE IS NULL!!");
				else if(cp->classType==CLASS_AUTO){
					error("TYPE IS AUTO!");
				}
				//E_VAR* var = new E_VAR(filePos,cpw->getMetadata());//TODO think of other way to represent
				dec.push_back(new Declaration(filePos,cp,nullptr,false, nullptr));
			}
			return new FunctionProto(toCall->getFullName(),dec,NULL);
		}*/
		const Token getToken() const override{
			return T_FUNC_CALL;
		};
		void write(ostream& f,String t="") const override{

			toCall->write(f);
			f << "(";
			bool first = true;
			for(auto& a:vals){
				if(first) first = false;
				else f << ", ";
				a->write(f);
			}
			f<<")";
			/*
			f<<"call(";
			toCall->write(f);
			f << ", [";
			bool first = true;
			for(auto& a:vals){
				if(first) first = false;
				else f << ", ";
				a->write(f);
			}
			f<<"])";
			 */
		}
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
			const AbstractClass* const cc = toCall->getReturnType();
			if(cc->classType==CLASS_CLASS){
				return toCall->getSelfClass(filePos);
			}
			return toCall->getFunctionReturnType(filePos,(const std::vector<Evaluatable*>)vals);
//			return toCall->getMetadata().getFunctionReturnType(filePos,generateFunctionProto());
		}
		Statement* simplify() override{
			std::vector<Statement*> g;
			for(auto a:vals) g.push_back(a->simplify());
			return new E_FUNC_CALL(filePos, toCall, g);
		}
		void collectReturns(std::vector<const AbstractClass*>& vals, const AbstractClass* const toBe) override final{}
		/*std::pair<std::vector<Value*>,Data*> getArgs(RData& a){
			assert(this);
			assert(&a);
			checkTypes();
			assert(toCall);
			assert(toCall->returnType);
			Data* d_callee;
			std::vector<Value*> Args;
			if(toCall->returnType==classClass){
				d_callee = toCall->getSelfClass()->constructors.get(generateFunctionProto(),filePos);
				assert(d_callee.getType()==R_FUNC);
			} else {
				auto tmp =toCall->getMetadata().getFunction(filePos,generateFunctionProto());
				d_callee = tmp.first;
				if(tmp.second==SCOPE_LOCAL_FUNC){

				}
			}
			FunctionProto* proto = d_callee.getFunctionType();
			if(d_callee.getType()!=R_FUNC && d_callee.getType()!=R_GEN){
				error("Cannot call function of non function/generator");
			}
			if(toCall->getToken()==T_LOOKUP){
				E_LOOKUP* T = (E_LOOKUP*)toCall;
				AbstractClass* tp = T->left->checkTypes();
				if(tp->hasFunction(T->right)){
					//TODO make better check (e.g. static functions)
					Value* loc;
					if(tp->layout==POINTER_LAYOUT || tp->layout==PRIMITIVEPOINTER_LAYOUT) loc = T->left->evaluate(a).getValue(a,filePos);//TODO check
					else{
						Value* v = T->left->evaluate(a).getValue(a,filePos);
						loc = a.builder.CreateAlloca(v->getType(),0);
						a.builder.CreateStore(v,loc);
					}
					Args.push_back(loc);
				}
			}
			for(unsigned int i = 0; i<vals.size(); i++){
				AbstractClass* t = proto->declarations[i]->classV->getSelfClass();
				if(vals[i]->checkTypes()==voidClass)
					Args.push_back( proto->declarations[i]->value->evaluate(a).castToV(a, t, filePos));
				else{
					Args.push_back( vals[i]->evaluate(a).castToV(a, t, filePos));
				}
				assert(Args.back() != NULL);
			}
			for(unsigned int i = vals.size(); i<proto->declarations.size(); i++){
				AbstractClass* t = proto->declarations[i]->classV->getSelfClass();
				Args.push_back( proto->declarations[i]->value->evaluate(a).castToV(a, t, filePos));
			}
			return std::pair<std::vector<Value*>,Data*>(Args,d_callee);
		}*/
		const Data* evaluate(RData& a) const override{
			const Data* func = toCall->evaluate(a);
			if(func->getReturnType()->classType==CLASS_CLASS){
				AbstractClass* ac = func->getMyClass(a,filePos);
				return ac->constructors.callFunction(a,filePos,(std::vector<Evaluatable*>)vals);
				/*std::vector<Data*> vec;
				for(auto& inner: vals) vec.push_back(inner->evaluate(a));
				return ac->constructors.call(a,filePos,vec);
				*/
			} else {
//				AbstractFunction* af = func->getFunction(filePos);
/*				std::vector<Data*> vec;
				for(auto& inner: vals) vec.push_back(inner->evaluate(a));
				return func->callFunction(a,filePos,vec);*/
				return func->callFunction(a,filePos,(std::vector<Evaluatable*>)vals);
			}
			/*
			if(toCall->getToken()==T_LAMBDAFUNC){
				lambdaFunction* temp = (lambdaFunction*)toCall;
				//TODO be aware that this makes something like
				/*
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
