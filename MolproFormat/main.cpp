#include "SingleBasisSet.hpp"

int main(int argc, char** argv){

	SingleBasisSet<double> A;
	//if(!A.importBasisSetGamessFormat("HCuN.basis")) return 1;
	//A.printBasisSet();
	A.importBasisSetMolproFormat("LiNAscc-pVTZ.basis");
	A.exportBasisSetMolproFormat ( "test.basis" );
//	A.printBasisSet();
	return 0;
}


