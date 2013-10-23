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
		const bool exists(String index) const{
			auto paired = mapping.find(index);
			if(paired!= mapping.end()) return true;
			if(super==NULL) return false;
			return super->exists(index);
		}
		ReferenceElement* getClassPointer(PositionID a, String name){
			return addPointer(a,name,DATA::getNull(),classClass);
		}
		ReferenceElement* getFuncPointer(PositionID a, String name){
			if(!exists(name)) return addPointer(a,name,DATA::getNull(),functionClass);
			else return findPointer(a,name);
		}
		ReferenceElement* addPointer(PositionID id, String index, DATA value, ClassProto* cla){
			if(mapping.find(index)!=mapping.end()){
				id.error("The variable "+index+" has already been defined in this scope");
			}
			ReferenceElement* nex = new ReferenceElement("",this, index,value, cla, funcMap());
			mapping.insert(std::pair<String,ReferenceElement*>(index, nex));
			return nex;
		}
		ReferenceElement* findPointer(PositionID a, String index,bool createIfNeeded=true) {
			auto paired = mapping.find(index);
			if(paired!= mapping.end())
				return paired->second;
			if(super!=NULL){
				auto tmp = super->findPointer(a, index,false);
				if(tmp!=NULL) return tmp;
			}
			if(createIfNeeded) return addPointer(a, index, DATA::getNull(),NULL);
			else return NULL;
		}
		ReferenceElement* getPointer(PositionID id, String index,bool top=true) {
			auto paired = mapping.find(index);
			if(paired!=mapping.end()) return paired->second;
			if(super!=NULL){
				auto tmp = super->getPointer(id,index,false);
				if(tmp!=NULL) return tmp;
			}
			if(!top) return NULL;
			cerr << "Could not resolve variable: " << index << flush << endl;
			write(cerr, "");
			cerr << endl << flush;
			id.error("Could not resolve variable: "+index);
			exit(0);
		}
		void write(ostream& a,String t) const override{
			a << "Module[" << flush;
			bool first = true;
			for(auto & b: mapping){
				if(first) first = false;
				else a << ", " << flush;
				a << b.first << ":";
				if(b.second->returnClass==NULL)
				a << "null";
				else a << b.second->returnClass->name;
			}
			a << "]|" << flush;
			if(super!=NULL) super->write(a,t);
		}
};

class LateResolve : public Resolvable{
	public:
		PositionID filePos;
		virtual ~LateResolve(){};
		LateResolve(OModule* m,String n, PositionID id): Resolvable(m,n),filePos(id){};
		ReferenceElement* resolve(){
			auto a =  module->getPointer(filePos, name);
			if(a==NULL) filePos.error("Could not resolve late pointer for "+name);
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
lmod->setDataLayout("p:64:64:64");
	  InitializeNativeTarget();
			exec = EngineBuilder(lmod).create();

			fpm = new FunctionPassManager(lmod);
			mpm = new PassManager();
			// Set up optimizers
			PassManagerBuilder pmb;
			pmb.Inliner = createFunctionInliningPass();
			pmb.OptLevel = 3;
			pmb.populateFunctionPassManager(*fpm);
			pmb.populateModulePassManager(*mpm);
			/*
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

		fpm->add(createFunctionInliningPass());

	fpm->doInitialization();*/
		}
#endif /* MODULE_HPP_ */
