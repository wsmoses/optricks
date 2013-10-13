/*
 * ImportStatement.hpp
 *
 *  Created on: Oct 11, 2013
 *      Author: wmoses
 */

#ifndef IMPORTSTATEMENT_HPP_
#define IMPORTSTATEMENT_HPP_

#include "Statement.hpp"
class ImportStatement:public Construct{
	public:
		String toImport;
		ImportStatement(PositionID id, String t):Construct(id,voidClass),toImport(t){};
		DATA evaluate(RData& r) override final{
			error("Cannot evaluate IMPORT");
			return NULL;
		}
		ClassProto* checkTypes(RData& r) override final{
			error("Cannot checkTypes of IMPORT");
			return NULL;
		}
		void registerFunctionArgs(RData& r) override final{
			error("Cannot registerFunctionArgs of IMPORT");
		}
		void collectReturns(RData& r, std::vector<ClassProto*>& vals){
		}
		void write(ostream& a, String s) const override final{
			a << "import" << "'" << toImport << "';" << endl << s;
		}
		void resolvePointers() override final{
			error("Cannot resolve pointers of IMPORT");
		}
		const Token getToken() const override final{
			return T_IMPORT;
		}
		void registerClasses(RData& r) override final{
			error("Cannot registerClasses of IMPORT");
		}
		void registerFunctionDefaultArgs() override final{

		}
		ImportStatement* simplify() override final{
			return this;
		}
};


#endif /* IMPORTSTATEMENT_HPP_ */
