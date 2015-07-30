#ifndef SINGLEBASISSET_H
#define SINGLEBASISSET_H

#include<map>
#include<vector>
#include<iostream>
#include<fstream>
#include<string>
#include<boost/regex.hpp>
//#include <regex>

using std::vector;
using std::map;
using std::pair;
using std::string;
using std::cerr;
using std::cout;
using std::endl;
using std::ifstream;
using std::ofstream;
using std::getline;
using boost::regex_replace;
using boost::regex_match;
using boost::regex_search;
using boost::regex;

template< typename T >
class SingleBasisSet{
private:
	map< vector< string> , vector < vector < vector < pair < T, T> > > > > content;
	bool getLandNoPrimitives(string str, int & L, int & N) const;
//	непонятно, надо имя или нет tring//значение пары: внешний вектор-его индексация-номер орб кв числа, промежуточный вектор-оболочка(одна), внутренний вектор-вектор пар-показатель экспоненты-коэффициент при экспоненте.  
public:
//	TODO операции с несколькими классами, объединение
	SingleBasisSet();
	~SingleBasisSet();
	void printBasisSet() const ;
	bool importBasisSetMolproFormat( const char * fileName);
	bool importBasisSetGamessFormat( const char * fileName);
	bool exportBasisSetMolproFormat( const char * fileName) const;
	bool exportBasisSetGamessFormat( const char * fileName) const;
};


regex shell("^[A-Z]\\s{1,3}\\d{1,2}");//оболочка
bool shellsearch (string str){
	if (regex_search(str,shell)) return true;
}
template< typename T >
bool SingleBasisSet<T>::getLandNoPrimitives(string str, int & L, int & N) const{
	regex shell("^\\s*([a-zA-Z])\\s+(\\d+)$");
	boost::smatch result;
/*	
	Идея следующая.
	1. Проверить, что строка отвечает регулярному выражению вида ^\s*([a-zA-Z])\s+(\d+)$', если все плохо - вернуть false
	2. Проверить, что результат один - это s или S (хоть прямым if'ом) - что распознается, иначе вернуть false
	3. для S - L=0, P - L=1, D L=2, ..... L = -1;
	4. Результат два просто передать в N через atoi. Если N=0 - выразить пользователю недоумение.
	5. вернуть true;
*/
	if(!regex_search(str,result,shell)) return false;
	int lTmp=-2;
	if (((result[1].str()).c_str())=="s" || ((result[1].str()).c_str())=="S" ) lTmp=0;
	if (((result[1].str()).c_str())=="p" || ((result[1].str()).c_str())=="P" ) lTmp=1;
	if (((result[1].str()).c_str())=="d" || ((result[1].str()).c_str())=="D" ) lTmp=2;
	if (((result[1].str()).c_str())=="f" || ((result[1].str()).c_str())=="F" ) lTmp=3;
	if (((result[1].str()).c_str())=="g" || ((result[1].str()).c_str())=="G" ) lTmp=4;
	if (((result[1].str()).c_str())=="h" || ((result[1].str()).c_str())=="H" ) lTmp=5;
	if (((result[1].str()).c_str())=="i" || ((result[1].str()).c_str())=="I" ) lTmp=6;
	if (((result[1].str()).c_str())=="k" || ((result[1].str()).c_str())=="K" ) lTmp=7;
	if (((result[1].str()).c_str())=="l") lTmp=8;
	if (((result[1].str()).c_str())=="L") lTmp=-1;
	
	if (lTmp==-2) return false;
	if (N==0) return false;
	N=atoi((result[2].str()).c_str());
	L=lTmp;
	return true;
}


regex S("^S\\s{3}\\d{1,2}");
regex P("^P\\s{3}\\d{1,2}");
regex D("^D\\s{3}\\d{1,2}");
regex F("^F\\s{3}\\d{1,2}");
regex G("^G\\s{3}\\d{1,2}");
regex L("^L\\s{3}\\d{1,2}");
int orbital (string str){
	if(regex_search(str,S)) return 0;
	if(regex_search(str,P)) return 1;
	if(regex_search(str,D)) return 2;
	if(regex_search(str,F)) return 3;
	if(regex_search(str,G)) return 4;
	if(regex_search(str,L)) return 5;
}


template< typename T >
SingleBasisSet<T>::SingleBasisSet(){
	content.clear();
}

template< typename T >
SingleBasisSet<T>::~SingleBasisSet(){}

template< typename T >
void SingleBasisSet<T>::printBasisSet() const{
	cout<<"Число элементов в базисном наборе: " << content.size() <<'\n';
	
}

template< typename T >
bool SingleBasisSet<T>::importBasisSetMolproFormat( const char * fileName){
	if ( !content.empty() ) {
		cerr << "Базисный набор уже задан, ошибка!\n";
		return false;
	}
	vector<string> elementLabel;
	vector < vector < vector < pair < T, T> > > > elementContent;
	elementLabel.push_back("carbon");
	content[elementLabel]=elementContent;
	
	return true;
}

template< typename T >
bool SingleBasisSet<T>::importBasisSetGamessFormat( const char * fileName){
	if ( !content.empty() ) {
		cerr << "Базисный набор уже задан, ошибка!\n";
		return false;
	}
	vector<string> elementLabel;
	vector < vector < vector < pair < T, T> > > > elementContent;
	ifstream inp (fileName);
	string str;
	bool isEndGood = false;
	bool isBeginGood = false;
	bool isBadLine = false;
	int noLine = 0;
	int *index = new int;
	int i = 0;
	vector<pair<double,double> > CiAi;
	regex empty("^\\s*$");//пустая строка
	regex emptyOrComment("^\\s*(!.*)*$");//пустая строка или коммент. (все ли правильно в этом рв?)
	regex firstLine("^\\s*\\$DATA\\s*$");//$DATA
	regex del("[A-Z]{4,}\\s*$");//название элемента. здесь вопрос-в рег выражении оказывается что друг за другом следуют подряд 4 или больше букв, в названии любого элемента это действительно присутствует, но это рег. выражение не контролирует случай, если последовательность букв не отвечает названию какого-либо элемента. 
	regex shell("^[A-Z]\\s{1,3}\\d{1,2}");//оболочка
	regex number("^\\s*(\\d+)((\\s*\\d+[\\.]?\\d+[EeDd]?[+-]?\\d+)*)(.*)");
	regex numer("\\d+[\\.]?\\d+[EeDd]?[+-]?\\d+");
	regex trash1("^[A-Z]{4,}\\d+.*");
	regex trash2("^[A-Z]{4,}\\s*\\w*.*$");
	regex end("^\\$END");
//	Проверка шапки файла
	while( getline(inp,str) ){
		noLine++;
		if(regex_search(str,emptyOrComment)) continue;  
		
//	Если строка пустая, то дальше
//	Если строка коммент, то дальше
//	Если строка стартовая, то смена флага и выход из while
//	выход из while
		if(regex_search(str,firstLine)) {
			isBeginGood = true;
			break;
		}
		isBadLine = true;
		break;
	}
	if(isBadLine){
		cout << "В строке " << noLine << " \""<<str<<"\"\n"<<" ошибка, чтение файла " << fileName << " не будет продолжено! " << endl;
		return false;
	}
	if(!isBeginGood){
		cout << "В файле " << fileName << " нет данных! " << endl;
		return false;
	}
/* Пример разбора строки
	getline(inp,str);

	const boost::sregex_iterator itEmpty;
	boost::sregex_iterator it(str.begin(),str.end(),regex("\\s*[^\\s]+"));
	elementLabel.clear();
	while (it != itEmpty ) elementLabel.push_back((*it++).str());
	content[elementLabel]=elementContent;
	return true;
*/



//	Заполнение элементов
	boost::smatch ourRes;
	double Ci, Ai;
	bool badIdentificator=false;
	while(getline(inp,str)) {
		noLine++;
		if(regex_search(str,del)){
			if(regex_search(str,trash1)||regex_search(str,trash2)) {
				badIdentificator=true;//или нет? может быть, что после названия элемента будет идти его порядковый
				break;			//номер
				}
			 elementLabel.push_back(str);//записываются полные названия элементов большими буквами. мб надо добавить еще
			 continue;			     //нечто идентифицирующее элемент
//	 new branch 'dima'
 
									}
		if(shellsearch(str)) {
			index[i]=orbital(str);
			i++;//как-то это криво
			continue;
			}
		if(regex_search(str,ourRes,number)){
					string str2(ourRes[2]);
					boost::sregex_iterator xIt(str2.begin(), str2.end(), numer);
                                        boost::sregex_iterator xInvalidIt;
						cout << *xIt << endl;
						xIt++;//какая то фигня с итераторами
						cout << *xIt << endl;
					
					
					 
			}
	}
	if(badIdentificator)
	cout << "В строке " << noLine << " \""<<str<<"\"\n" << "ошибка, не существует химического элемента с таким названием, чтениефайла не будет продолжено" << endl;  

//	Проверка корректного завершения файла
	int endT=0;
	while( getline(inp,str) ){
		if(regex_search(str,end)) break;
		endT++;//строго, даже строки с символами-разделятелями нельзя
		
	}
	if(endT){
	cout << "После базисного набора и идентификатора $END окончания файла присутствуют непустые строки. ";
	cout << "Если Вам кажется, что строки ниже пустые, проверьте, нет ли в строках, идущих ниже строки \"$END\" символов-разделителей. ";
	}



	inp.close();
	delete [] index;
//	Формат файла
//	{пустая строчка или строчка из комментариев}
//	{начальная строчка}
//	блок{
//	строка комментариев или любая строка из слов, которые будут определять элемент
//	блок{
//		Буква, целое [, комментарий] 
//		целое, double, double, [double если буква = L] [!, комментарий]
//	}
//	пустая строка или конечная строчка
//	}
//	пустая строка *
//	конечная строка*
//	{пустая строчка или строка комментариев}

	elementLabel.push_back("CARBON");
	content[elementLabel]=elementContent;
	return true;
}

template< typename T >
bool SingleBasisSet<T>::exportBasisSetMolproFormat( const char * fileName) const{
	return true;	
}

template< typename T >
bool SingleBasisSet<T>::exportBasisSetGamessFormat( const char * fileName) const{
	return true;
}


#endif

