#include <stdio.h>
int main(){
	int a , b, c;
	a =1, b =2, c =3;

	printf("%d %d %d\n", a, b, c);
	a += b += c;
	
	printf("%d %d %d\n", a, b, c);

	return 0;

}
