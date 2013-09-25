/*
 * Module.hpp
 * Contains list of local variables for resolution
 *  Created on: Jul 26, 2013
 *      Author: wmoses
 */

#ifndef MODULE_HPP_
#define MODULE_HPP_

#include "Stackable.hpp"
#include "../containers/all.hpp"
#include "../primitives/oobjectproto.hpp"

class OModule : public Stackable{
	public:
		virtual ~OModule(){};
		OModule* super;
		std::map<String, ReferenceElement*> mapping;
		OModule(const OModule& c) = delete;
		OModule(OModule* before): mapping(){
			super = before;
		}
		const Token getToken() const override{
			return T_MODULE;
		}
		const int exists(String index) const{
			const OModule* search = this;
			int level = 0;
			while(search!=NULL){
				auto paired = search->mapping.find(index);
				if(paired== search->mapping.end()){
					search = search->super;
					level++;
				} else {
					return level;
				}
			}
			return -1;
		}
		/*void ssetPointer(PositionID a, String index, DATA value, ClassProto* cl, FunctionProto* fun, ClassProto* selfCl,AllocaInst* al){
			ReferenceElement* p = findPointer(a, index);
			p->llvmObject = value;
			p->returnClass = cl;
			p->function = fun;
			p->selfClass = selfCl;
			p->llvmLocation = al;
		}*/
		ReferenceElement* getFuncPointer(PositionID a, String name){
			if(exists(name)==-1) return addPointer(a,name,NULL,functionClass,NULL,NULL,0);
			else return findPointer(a,name);
		}
		ReferenceElement* addPointer(PositionID a, String index, DATA value, ClassProto* cla, ClassProto* selfCl, AllocaInst* al, unsigned int level=0){
			if(level == 0){
				if(mapping.find(index)!=mapping.end()){
					todo("The variable "+index+" has already been defined in this scope", a);
				}
				auto nex = new ReferenceElement("",this, index,value, cla, funcMap(), selfCl, al);
				mapping.insert(std::pair<String,ReferenceElement*>(index, nex));
				return nex;
			} else {
				if(super==NULL){
					cerr << "Null module to add pointer to" << endl << flush;
					exit(1);
				}
				else
				return super->addPointer(a, index, value, cla, selfCl, al, level-1);
			}
		}
		ReferenceElement* findPointer(PositionID a, String index) {
			const OModule* search = this;
			while(search!=NULL){
				auto paired = search->mapping.find(index);
				if(paired== search->mapping.end()){
					search = search->super;
				} else {
					return paired->second;
				}
			}
			return addPointer(a, index, NULL,NULL, NULL,NULL);
		}
		ReferenceElement* getPointer(PositionID id, String index) {
			OModule* search = this;
			while(search!=NULL){
				auto paired = search->mapping.find(index);
				if(paired== search->mapping.end()){
					search = search->super;
				} else {
					return paired->second;
				}
			}
			cerr << "Could not resolve variable: " << index << flush << endl;
			write(cerr, "");
			cerr << endl << flush;
			todo("Could not resolve variable: "+index,id);
			exit(0);
		}
		void write(ostream& a,String t) const override{
			a << "Module[" << flush;
			bool first = true;
			for(auto & b: mapping){
				if(first) first = false;
				else a << ", " << flush;
				a << b.first << flush;
			}
			a << "]|" << flush;
			if(super!=NULL) super->write(a,t);
		}
};

ReferenceElement* ReferenceElement::resolve(){
	return this;
	//if(module==NULL) return this;
	//return module->mapping[name];
};

class LateResolve : public Resolvable{
	public:
		PositionID filePos;
		virtual ~LateResolve(){};
		LateResolve(OModule* m,String n, PositionID id): Resolvable(m,n),filePos(id){};
		ReferenceElement* resolve(){
			auto a =  module->getPointer(filePos, name);
			if(a==NULL) todo("Could not resolve late pointer for "+name,filePos);
			return a;
		}
};

OModule* LANG_M = new OModule(NULL);

RData::RData():
//			module(new OModule(LANG_M)),
			jumps(),
			guarenteedReturn(false),
			lmod(new Module("main",getGlobalContext())),
			builder(IRBuilder<>(lmod->getContext()))
			{

	  InitializeNativeTarget();
			exec = EngineBuilder(lmod).create();
			fpm = new FunctionPassManager(lmod);
	fpm->add(new DataLayout(*exec->getDataLayout()));
	// Provide basic AliasAnalysis support for GVN.

	fpm->add(createBasicAliasAnalysisPass());
	// Do simple "peephole" optimizations and bit-twiddling optzns.
	fpm->add(createInstructionCombiningPass());
	// Reassociate Statements.
	fpm->add(createReassociatePass());
	// Eliminate Common SubStatements.
	fpm->add(createGVNPass());
	// Simplify the control flow graph (deleting unreachable blocks, etc).
	fpm->add(createCFGSimplificationPass());
///HERE ARE NEW ONES
	///*
	fpm->add(createCFGSimplificationPass()); // Clean up disgusting code
	    fpm->add(createPromoteMemoryToRegisterPass());// Kill useless allocas

	    fpm->add(createInstructionCombiningPass()); // Cleanup for scalarrepl.
	    fpm->add(createScalarReplAggregatesPass()); // Break up aggregate allocas
	    fpm->add(createInstructionCombiningPass()); // Cleanup for scalarrepl.
	    fpm->add(createJumpThreadingPass());        // Thread jumps.
	    fpm->add(createCFGSimplificationPass());    // Merge & remove BBs
	    fpm->add(createInstructionCombiningPass()); // Combine silly seq's

	    fpm->add(createCFGSimplificationPass());    // Merge & remove BBs
	    fpm->add(createReassociatePass());          // Reassociate expressions

	    fpm->add(createEarlyCSEPass()); //// ****

	    fpm->add(createLoopIdiomPass()); //// ****
	    fpm->add(createLoopRotatePass());           // Rotate loops.
	    fpm->add(createLICMPass());                 // Hoist loop invariants
	    fpm->add(createLoopUnswitchPass());         // Unswitch loops.
	    fpm->add(createInstructionCombiningPass());
	    fpm->add(createIndVarSimplifyPass());       // Canonicalize indvars
	    //fpm->add(createLoopDeletionPass());         // Delete dead loops
	    fpm->add(createLoopUnrollPass());           // Unroll small loops
	    //fpm->add(createLoopStrengthReducePass());   // (jwb added)

	    fpm->add(createInstructionCombiningPass()); // Clean up after the unroller
	    fpm->add(createGVNPass());                  // Remove redundancies
	    //fpm->add(createMemCpyOptPass());            // Remove memcpy / form memset
	    fpm->add(createSCCPPass());                 // Constant prop with SCCP

	    // Run instcombine after redundancy elimination to exploit opportunities
	    // opened up by them.
	    fpm->add(createSinkingPass()); ////////////// ****
	    fpm->add(createInstructionSimplifierPass());///////// ****
	    fpm->add(createInstructionCombiningPass());
	    fpm->add(createJumpThreadingPass());         // Thread jumps
	    fpm->add(createDeadStoreEliminationPass());  // Delete dead stores

	    fpm->add(createAggressiveDCEPass());         // Delete dead instructions
	    fpm->add(createCFGSimplificationPass());     // Merge & remove BBs
//*/
	fpm->doInitialization();
		}
#endif /* MODULE_HPP_ */
