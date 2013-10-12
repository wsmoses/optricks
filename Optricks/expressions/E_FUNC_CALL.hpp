#ifndef E_FUNC_CALL_HPP_
#define E_FUNC_CALL_HPP_
#include "../constructs/Statement.hpp"
#include "../primitives/ofunction.hpp"

#define E_FUNC_CALL_C_
class E_FUNC_CALL : public Statement{
	public:
		Statement* toCall;
		std::vector<Statement*> vals;
		virtual ~E_FUNC_CALL(){};
		E_FUNC_CALL(PositionID a, Statement* t, std::vector<Statement*> val) : Statement(a,NULL),
				toCall(t), vals(val){
		};
		Constant* getConstant(RData& a) override final {
			return NULL;
		}
		ClassProto* getSelfClass() override final{ error("Cannot get selfClass of construct "+str<Token>(getToken())); return NULL; }
		FunctionProto* generateFunctionProto(RData& r) const{
			std::vector<Declaration*> dec;
			for(auto& a:vals){
				ClassProto* cp=a->checkTypes(r);
				if(cp==NULL) error("TYPE IS NULL!!");
				else if(cp==autoClass){
					error("TYPE IS AUTO!");
				}
				ClassProtoWrapper* cpw = new ClassProtoWrapper(cp);
				E_VAR* var = new E_VAR(filePos,cpw->getMetadata(r));
				dec.push_back(new Declaration(filePos,cpw,var,false, NULL));
			}
			return new FunctionProto(toCall->getFullName(),dec,NULL);
		}
		ReferenceElement* getMetadata(RData& r) override final {
			error("getting metadata of func-call");
			return NULL;
		}
		Value* getLocation(RData& a) override final{
			//TODO
			return NULL;
		}
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
		void registerClasses(RData& r) override final{
			toCall->registerClasses(r);
			for(auto &a : vals) a->registerClasses(r);
		}
		void registerFunctionArgs(RData& r) override final{
			toCall->registerFunctionArgs(r);
			for(auto &a : vals) a->registerFunctionArgs(r);
		}
		void registerFunctionDefaultArgs() override final{
			toCall->registerFunctionDefaultArgs();
			for(auto &a : vals) a->registerFunctionDefaultArgs();
		}
		void resolvePointers() override final{
			toCall->resolvePointers();
			for(auto &a : vals) a->resolvePointers();
		}
		String getFullName() override final{
			return toCall->getFullName()+"(...)";
		}
		ClassProto* checkTypes(RData& r) override{
			toCall->checkTypes(r);
			if(toCall->returnType==classClass){
				for(unsigned int i = 0; i<vals.size(); i++){
								vals[i]->checkTypes(r);
				}
				return returnType = toCall->getMetadata(r)->selfClass;
			} //TODO oh -- constructor
			FunctionProto* proto = toCall->getMetadata(r)->funcs.get(generateFunctionProto(r),filePos).second;
			if(proto==NULL) error("Non-existent function prototype");
			return returnType = proto->returnType;
		}
		Statement* simplify() override{
			auto tem = toCall->simplify();
			std::vector<Statement*> g;
			for(auto a:vals) g.push_back(a->simplify());
			return new E_FUNC_CALL(filePos, tem, g);
		}
//		Value* getLocation(RData& a) override{
	//TODO make data a struct {Value (data), Value (location) }
//		}
		void collectReturns(RData& r, std::vector<ClassProto*>& vals){
}
		DATA evaluate(RData& a) override{
			lambdaFunction* temp = dynamic_cast<lambdaFunction*>(toCall);
			if(temp!=NULL){
				//TODO be aware that this makes something like
				/*
				 * for(int i=0; i<7; i+=1) printi((lambda int z: z*i)(i)) // VALID
				 * yet something like
				 * for(int i=0; i<7; i+=1){
				 *  auto tmp = (lambda int z: z*i)
				 *  printi(tmp(i))
				 *  } 													 // INVALID
				 */
				//for(int i=0; i<7; i+=1){ auto tmp = (lambda int z: z*z) printi(tmp(i)) }
				for(unsigned int i = 0; i<temp->prototype->declarations.size(); i++){
					Declaration* decl = temp->prototype->declarations[i];
					if(i<vals.size()) decl = new Declaration(decl->filePos, decl->classV, decl->variable, false, vals[i]);
					else if(decl->value==NULL || decl->value->getToken()==T_VOID) error("No argument for lambda function!");
					decl->evaluate(a);
				}
				return temp->ret->evaluate(a);
			}
			std::pair<Value*,FunctionProto*> funcs = (toCall->returnType==classClass)?(
					toCall->getMetadata(a)->selfClass->constructors.get(generateFunctionProto(a),filePos))
					:(toCall->getMetadata(a)->funcs.get(generateFunctionProto(a),filePos));
			FunctionProto* proto = funcs.second;
			Value* callee = funcs.first;
			std::vector<Value*> Args;
			if(auto T=dynamic_cast<E_LOOKUP*>(toCall)){
				auto tp = T->left->checkTypes(a);
				if(tp->hasFunction(T->right)){
					//TODO make better check (e.g. static functions)
					DATA loc = (tp->isPointer)?(T->left->evaluate(a)):(T->left->getLocation(a));
					if(loc==NULL){
						//TODO allow for second function of type instead of type*
						//thereby reducing number of allocas/loads/stores
						//error("Could not find location of object to insert into class function - using automatic pointer creation",false);
						Function *TheFunction = a.builder.GetInsertBlock()->getParent();
						IRBuilder<> TmpB(&TheFunction->getEntryBlock(),TheFunction->getEntryBlock().begin());
						loc = TmpB.CreateAlloca(T->left->returnType->getType(a), 0,"tmp");
						a.builder.CreateStore(T->left->evaluate(a),loc);
					}
					Args.push_back(loc);
				}
			}
			//cout << proto->toString() << endl << flush;
			for(unsigned int i = 0; i<vals.size(); i++){
				ClassProto* t = proto->declarations[i]->classV->getMetadata(a)->selfClass;
				if(vals[i]->returnType==voidClass)
					Args.push_back( proto->declarations[i]->value->returnType->castTo(a, proto->declarations[i]->value->evaluate(a), t));
				else
					Args.push_back(vals[i]->returnType->castTo(a, vals[i]->evaluate(a), t));
				if (Args.back() == 0) error("Error in eval of args");
			}
			for(unsigned int i = vals.size(); i<proto->declarations.size(); i++){
				ClassProto* t = proto->declarations[i]->classV->getMetadata(a)->selfClass;
				Args.push_back( proto->declarations[i]->value->returnType->castTo(a, proto->declarations[i]->value->evaluate(a), t));
			}

			if(returnType==voidClass) return a.builder.CreateCall(callee, Args);
			else{
				auto t = a.builder.CreateCall(callee, Args, "calltmp");
				return t;
			}
		}
};

/*
DATA ClassProto::construct(RData& r, E_FUNC_CALL* call) const{
	if(nativeConstructor!=NULL) return nativeConstructor(r,call->vals,call->filePos,name);
	else{
		auto func = constructors.get(call->generateFunctionProto(r),call->filePos,r);
		FunctionProto* proto = func.second;
		Value* callee = func.first;
		Type* t = getType(r);
		DATA val = get
		//TODO finish
	}
}*/

#endif /* E_FUNC_CALL_HPP_ */
