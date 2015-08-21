#include "SingleBasisSet.hpp"

int main(int argc, char** argv){

	SingleBasisSet<double> A;
	//if(!A.importBasisSetGamessFormat("HCuN.basis")) return 1;
	//A.printBasisSet();
	A.importBasisSetMolproFormat("goodBasisStrings.basis");
//	A.exportBasisSetMolproFormat("1HNiF.basis");
	A.printBasisSet();
	return 0;
}


