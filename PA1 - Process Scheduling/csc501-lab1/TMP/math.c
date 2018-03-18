#include <stdio.h>
#include "math.h"

#define RAND_MAX 32767

//function to calculate 'a' raised to power 'b'
double pow(double base, double exp){
	double result = 1;
	int i;
	if(exp==0)
		return result;
	else if(exp>0){
		for(i=0;i<exp;i++)
			result = result * base;
		return result;
	}
	else{
		for(i=0;i>exp;i--)
			result = result / base;
		return result;
	}
}

//calculate natural logarithm of 'x'
//Taylor series is used for this
double log(double x){
	if(x<0)
		return -1;
	int n = 20;
	double result = 0;
	int i;
	for(i=0;i<n;i++){		
		result = result + (pow(-1,i)*pow((x-1),(i+1))/(i+1));
	}
	return result;

}

//generate exponential distributed values
double expdev(double lambda){
	double dummy;
	do{

		dummy = (double) rand()/32767;
	}while(dummy==0.0);

	return -log(dummy)/lambda;
}
/*
int main(){
	printf("log(2): %f\n",log(2));
	printf("log(4): %f\n",log(4));
	printf("log(10): %f\n",log(10));
	printf("log(20): %f\n",log(20));
	printf("log(e): %f\n",log(2.71828));
	printf("pow(0,1): %f\n",pow(0,1));
	printf("pow(2,3): %f\n",pow(2,3));
	printf("pow(10,-5): %f\n",pow(10,-5));
	printf("pow(-20,-1): %f\n",pow(-20,-1));
	printf("expdev(0.1): %f\n",expdev(0.1));
	printf("expdev(0.1): %f\n",expdev(0.1));
	printf("expdev(0.1): %f\n",expdev(0.1));
	printf("log(e): %f\n",log(2.71828));
	return 0;
}*/
