/*
 * ImportStatement.hpp
 *
 *  Created on: Oct 11, 2013
 *      Author: wmoses
 */

#ifndef IMPORTSTATEMENT_HPP_
#define IMPORTSTATEMENT_HPP_

#include "../language/statement/Statement.hpp"
class ImportStatement:public ErrorStatement{
	public:
		String toImport;
		ImportStatement(PositionID id, String t):ErrorStatement(id),toImport(t){};
		const Data* evaluate(RData& r) const override final{
			filePos.compilerError("Cannot evaluate IMPORT");
			exit(1);
		}
		const AbstractClass* getFunctionReturnType(PositionID id, const std::vector<Evaluatable*>& args)const{
			id.error("import-statement cannot act as function");
			exit(1);
		}
		const AbstractClass* getReturnType() const override final{
			filePos.compilerError("Cannot checkTypes of IMPORT");
			exit(1);
		}
		//bool hasCastValue(AbstractClass* a){
		//	return false;
		//}

		void collectReturns(std::vector<const AbstractClass*>& vals, const AbstractClass* const toBe) override final{
		}
		void write(ostream& a, String s) const override final{
			a << "import" << "'" << toImport << "';" << endl << s;
		}
		const Token getToken() const override final{
			return T_IMPORT;
		}
		void registerClasses() const override final{
		}
		void registerFunctionPrototype(RData& r) const override final{
		}
		void buildFunction(RData& r) const override final{
		}
		ImportStatement* simplify() override final{
			return this;
		}
};


#endif /* IMPORTSTATEMENT_HPP_ */
