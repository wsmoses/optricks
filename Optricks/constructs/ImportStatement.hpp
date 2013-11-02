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
			return DATA::getNull();
		}
		ClassProto* checkTypes(RData& r) override final{
			error("Cannot checkTypes of IMPORT");
			return NULL;
		}
		void registerFunctionPrototype(RData& r) override final{
		}
		void collectReturns(RData& r, std::vector<ClassProto*>& vals, ClassProto* toBe) override final{
		}
		void write(ostream& a, String s) const override final{
			a << "import" << "'" << toImport << "';" << endl << s;
		}
		const Token getToken() const override final{
			return T_IMPORT;
		}
		void registerClasses(RData& r) override final{
			error("Cannot registerClasses of IMPORT");
		}
		void buildFunction(RData& r) override final{
		}
		ImportStatement* simplify() override final{
			return this;
		}
};


#endif /* IMPORTSTATEMENT_HPP_ */
