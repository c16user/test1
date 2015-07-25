#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iterator>
#include <boost/regex.hpp>
#include <cstdlib>
int main()
{
std::ifstream inp("HCuN.basis");
std::ofstream out("output.txt");
std::string str;
using boost::regex;
using boost::regex_replace;  // странно, но предыдущий неймлист ее включает
using std::string;
regex a("!");//комменты в начале файла
regex b("((\\w|\\s){3,}?)((\\d|\\s){2,}?).*");//содержательная часть файла
regex name("\\w{4,}");//полные названия элементов и числа
regex dollar("\\$|\\*");//$data $end
//regex h("[a-z]{4,}?");
regex del("[A-Z]{4,}");//название элемента
regex empt("");
regex head("^\\w\\s{3}\\d{1,2}");
//regex headS("^S\\s{3}\\d{1,2}");
regex numbers ("\\s\\d{1,2}\\s*\\d*\\.\\s*.*");
regex ignore("^[A-Z]\\s{1,3}\\d{1,2}");
regex start("\\s\\d{1,2}.*");//первые символы строк, которые надо вытащить
string str_1st;
string str_2nd;
string str_3rd;
string str_4th;
regex first("\\s*\\d*\\..*");//это и ниже-регулярные выражения для вывода на экран только определенного столбца из файла
regex second_1("^\\s{1,2}\\d{1,2}\\s{2,}");
regex second_2("\\s{5,}.*");
regex third_1("^\\s{1,2}\\d{1,2}\\s{2,}\\d*\\.\\d*\\s{5,}");
regex third_2("\\s{5,}(-|\\d*).*$");
regex fourth_1("^(-|\\d{1})\\d{1,}\\.\\d{1,}(E|D)?-?\\d{1,}");
std::cout.precision(10);
out.setf(std::ios::fixed,std::ios::floatfield);
std::cout.setf(std::ios::fixed,std::ios::floatfield);
std::vector<int> arr1;//этот и 2 вектора ниже-для хранения эл-тов 1,2 и 3-го столбцов соответственно
std::vector<double> arr2;
std::vector<double> arr3;
while(std::getline(inp,str)){
if(((regex_search(str,b)) || regex_search(str,name)) && !((regex_search(str,a))||regex_search(str,dollar))){//это первый вариант, выводятся названия элементы а дальше сплошняком числа
if(regex_search(str,del)){
std::cout << str << std::endl;
continue;
}
if(regex_search(str,ignore)){
std::cout << str << std::endl;
arr1.clear();
arr2.clear();
arr3.clear();
continue;
}
if(regex_search(str,start)){
//////////////////////////////////////////////////
//str_1st=regex_replace(str,first,empt);
//arr1.insert(arr1.end(), atoi(str_1st.c_str());
//std::cout << arr1.back() << std::endl; 
//std::cout << str_1st << " " << std::endl;
//////////////////////////////////////////////////////////////////
//str_2nd=regex_replace(str,second_1,empt);
//str_2nd=regex_replace(str_2nd,second_2,empt);
//arr2.insert(arr2.end(), atof(str_2nd.c_str()));
//std::cout << arr2.back() << std::endl;//вывод последнего записанного элемента
//std::cout << str_2nd << std::endl; //" -------> " << q << std::endl;
///////////////////////////////////////////////////////////////////////////////////////
str_3rd=regex_replace(str,third_1,empt);
//std::cout << str_3rd << std::endl;
str_4th=regex_replace(str_3rd,fourth_1,empt);//так быстрее, убрать один из двух столбцов, которые получатся после предыдущей строки
std::cout << str_4th << std::endl;
//str_3rd=regex_replace(str_3rd,third_2,empt);
//arr3.insert(arr3.end(), atof(str_3rd.c_str()));
//out << arr3.back() << std::endl;
//std::cout << str_3rd <<  "--------->" << atof(str_3rd.c_str())*2 << std::endl;
//////////////////////////////////////////////*////////////////////////////////////////
}
}

}
}





