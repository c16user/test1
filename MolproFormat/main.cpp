#include "SingleBasisSet.hpp"

int main(int argc, char** argv){

	SingleBasisSet<double> A;
	//if(!A.importBasisSetGamessFormat("HCuN.basis")) return 1;
	//A.printBasisSet();
	//A.exportBasisSetMolproFormat("3.basis");
	A.importBasisSetMolproFormat("2cc-pVTZ.basis");
	//A.printBasisSet();
	return 0;
}


