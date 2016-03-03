
#include <stdio.h>
#include <iconv.h>
#include <stdlib.h>
#include <string.h>




int main(int argc, char *argv[])
{

  
  
		char *str = "й";
		
		int i;
		for (i = 0; i < strlen(str); i++)
		{
		
			uint j = (uint)str[i];
			
			
			if ( (j >= 192) && (j <= 255) )
			{
				j = j + 848;
				
			}
			else if(j == 184)
			{
				j = 1105;
			}
			
			
			
			printf("%04x ",j);	
		}
		

	return 0;
}

