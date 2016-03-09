#include "CString.h"

int main(int argc, char *argv[])
{
	CString sql("haha");  
    sql.print();  
  
      
    CString sql1(std::string("hehe"));  
    sql1.print();  
      
    CString sql2(sql);  
    sql2.print();  
  
    CString sql3 = sql1 + sql2;  
    sql3.print();  
  
    char strd[] = "xxxxx";  
    CString sql4 = sql + strd;  
    sql4.print();  
      
    std::string strstr = "dddddd";  
    CString sql5 = sql + strstr;  
    sql5.print();  
  
    return 0;
}
