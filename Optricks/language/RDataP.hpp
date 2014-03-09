/*
 * RDataP.hpp
 *
 *  Created on: Mar 3, 2014
 *      Author: Billy
 */

#ifndef RDATAP_HPP_
#define RDATAP_HPP_

#include "RData.hpp"
#include "./class/AbstractClass.hpp"
#include "./data/VoidData.hpp"
BasicBlock* RData::getBlock(String name, JumpType jump, BasicBlock* bb, const Data* val, PositionID id, std::pair<BasicBlock*,BasicBlock*> resume){
	if(name==""){
		if(jump==RETURN || jump==YIELD){
			for(int i = jumps.size()-1; ; i--){
				if(jumps[i]->toJump==GENERATOR){
					if(jumps[i]->returnType->classType==CLASS_VOID){
						if(val->type!=R_VOID && val->getReturnType()->classType!=CLASS_VOID) id.error("Cannot return something in function requiring void");
						jumps[i]->endings.push_back(std::pair<BasicBlock*,const Data*>(bb, VOID_DATA));
					}
					else jumps[i]->endings.push_back(std::pair<BasicBlock*,const Data*>(bb, val->castTo(*this, jumps[i]->returnType, id)));
					jumps[i]->resumes.push_back(resume);
					return jumps[i]->end;
				}
				if(i <= 0){
					cerr << "Error could not find returning block" << endl << flush;
					return NULL;
				}
			}
		} else {
			for(int i = jumps.size()-1; ; i--){
				if(jumps[i]->toJump == LOOP){
					//jumps[i]->endings.push_back(std::pair<BasicBlock*,Value*>(bb,val));
					return (jump==BREAK)?(jumps[i]->end):(jumps[i]->start);
				}
				if(i <= 0){
					cerr << "Error could not find continue/break block" << endl << flush;
					return NULL;
				}
			}
		}
	} else {
		cerr << "Error not done yet2" << endl << flush;
		exit(1);
		return NULL;
	}
}

#endif /* RDATAP_HPP_ */
