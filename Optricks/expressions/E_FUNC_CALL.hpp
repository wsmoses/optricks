#ifndef E_FUNC_CALL_HPP_
#define E_FUNC_CALL_HPP_
#include "../constructs/Statement.hpp"
#include "../primitives/ofunction.hpp"

#define E_FUNC_CALL_C_
//TODO if calling variables are all constants -- inline the function call
//TODO make constructors into generators
class E_FUNC_CALL : public Statement{
	public:
		Statement* toCall;
		std::vector<Statement*> vals;
		virtual ~E_FUNC_CALL(){};
		E_FUNC_CALL(PositionID a, Statement* t, std::vector<Statement*> val) : Statement(a,NULL),
				toCall(t), vals(val){
		};
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
		void registerFunctionPrototype(RData& r) override final{
			toCall->registerFunctionPrototype(r);
			for(auto &a : vals) a->registerFunctionPrototype(r);
		}
		void buildFunction(RData& r) override final{
			toCall->buildFunction(r);
			for(auto &a : vals) a->buildFunction(r);
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
				return returnType = toCall->getSelfClass(r);
			}
			DATA d = toCall->getMetadata(r)->funcs.get(generateFunctionProto(r),r,filePos);
			FunctionProto* proto = d.getFunctionType();
			if(proto==NULL) error("Non-existent function prototype");
			if(d.getType()==R_GEN) return returnType = proto->getGeneratorType(r);
			return returnType = proto->returnType;
		}
		Statement* simplify() override{
			auto tem = toCall->simplify();
			std::vector<Statement*> g;
			for(auto a:vals) g.push_back(a->simplify());
			return new E_FUNC_CALL(filePos, tem, g);
		}
		void collectReturns(RData& r, std::vector<ClassProto*>& vals, ClassProto* toBe) override final{}
		std::pair<std::vector<Value*>,DATA> getArgs(RData& a){

			DATA d_callee = (toCall->returnType==classClass)?(
					toCall->getSelfClass(a)->constructors.get(generateFunctionProto(a),a, filePos))
					:(toCall->getMetadata(a)->funcs.get(generateFunctionProto(a),a, filePos));
			FunctionProto* proto = d_callee.getFunctionType();
			if(d_callee.getType()!=R_FUNC && d_callee.getType()!=R_GEN){
				error("Cannot call function of non function/generator");
			}
			std::vector<Value*> Args;
			if(toCall->getToken()==T_LOOKUP){
				E_LOOKUP* T = (E_LOOKUP*)toCall;
				ClassProto* tp = T->left->checkTypes(a);
				if(tp->hasFunction(T->right)){
					//TODO make better check (e.g. static functions)
					Value* loc;
					if(tp->layoutType==POINTER_LAYOUT || tp->layoutType==PRIMITIVEPOINTER_LAYOUT) loc = T->left->evaluate(a).getValue(a);//TODO check
					else{
						loc = T->left->evaluate(a).toLocation(a).getMyLocation();
					}
					Args.push_back(loc);
				}
			}
			//cout << proto->toString() << endl << flush;
			for(unsigned int i = 0; i<vals.size(); i++){
				ClassProto* t = proto->declarations[i]->classV->getSelfClass(a);
				if(vals[i]->checkTypes(a)==voidClass)
					Args.push_back( proto->declarations[i]->value->evaluate(a).castTo(a, t, filePos).getValue(a) );
				else{
					Args.push_back( vals[i]->evaluate(a).castTo(a, t, filePos).getValue(a) );
				}
				assert(Args.back() != NULL);
			}
			for(unsigned int i = vals.size(); i<proto->declarations.size(); i++){
				ClassProto* t = proto->declarations[i]->classV->getSelfClass(a);
				Args.push_back( proto->declarations[i]->value->evaluate(a).castTo(a, t, filePos).getValue(a) );
			}
			return std::pair<std::vector<Value*>,DATA>(Args,d_callee);
		}
		DATA evaluate(RData& a) override{
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
			std::pair<std::vector<Value*>,DATA > tempVal = getArgs(a);
			std::vector<Value*> &Args = tempVal.first;
			DATA &d_callee = tempVal.second;
			if(d_callee.getType()==R_GEN){
				//TODO allow for auto tmp = "hello".iterator(), storing "hello" in temp struct
				ClassProto* cla = d_callee.getFunctionType()->getGeneratorType(a);
				Value* mine = cla->generateData(a).getValue(a);
				for(unsigned int i = 0; i<Args.size(); i++){
					mine = a.builder.CreateInsertValue(mine, Args[i], ArrayRef<unsigned>(std::vector<unsigned>({i})));
				}
				return DATA::getConstant(mine, cla);
			}
			Function* callee = d_callee.getMyFunction();
			if(returnType==voidClass){
				a.builder.CreateCall(callee, Args);
				return DATA::getNull();
			}
			else{
				Value* t = a.builder.CreateCall(callee, Args, "calltmp");
				return DATA::getConstant(t, returnType);
			}
		}
};

#endif /* E_FUNC_CALL_HPP_ */
