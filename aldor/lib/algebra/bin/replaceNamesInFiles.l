%{
#include <stdio.h>

FILE *fic;


%}



%%

MonogenicLinearArithmeticType {                                           
                fprintf(fic, "%s", "UnivariateFreeLinearArithmeticType");  
                  }                                                        
MonogenicAlgebra {                                                        
                  fprintf(fic, "%s", "UnivariateFreeRing");                
                  }                                                        
UnivariatePolynomialAlgebra {                                             
                  fprintf(fic, "%s", "UnivariatePolynomialRing");          
                    }                                                      
UnivariateTaylorSeriesCategory {                                          
                   fprintf(fic, "%s", "UnivariateTaylorSeriesType");       
                   }                                                       
UnivariatePolynomialCategory {                                            
                   fprintf(fic, "%s", "UnivariatePolynomialAlgebra");      
                  }                                                        

[\n]             fprintf(fic, "%s", "\n");
.                fprintf(fic, "%s", yytext);


%%

int main() {

     fic = fopen("BeaujolaisNouveau.as","w");
     
     yylex();

     fclose(fic);

     return 0;
}

