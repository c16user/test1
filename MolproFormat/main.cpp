#include "SingleBasisSet.hpp"

int main(int argc, char** argv){

	SingleBasisSet<double> A;
	//if(!A.importBasisSetGamessFormat("HCuN.basis")) return 1;
	//A.printBasisSet();
	A.importBasisSetMolproFormat("6-311+G.basis");
	A.exportBasisSetMolproFormat("a.basis");
//	A.printBasisSet();
	return 0;
}


