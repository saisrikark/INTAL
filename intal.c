#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include "intal.h"
#include <string.h>

struct integer_of_arb_length{
	unsigned int n;	
	int *integer; 			
};
typedef struct integer_of_arb_length INTAL;

void *mul_with_karatsuba(void *intal1, void *intal2);
void *intal_dup(void *intal);
void remove_leading_zeroes(void *intal);
void *intal_lsl(void *intal, void *shift);
void compareable(void *intal1, void *intal2);
void *intal_lsr(void *intal1, int n);
void *intal_mul_divide(void *intal1, void *intal2);
void *intal_mul_mod(void *intal1, void *intal2);
void *intal_modulo(void *intal1, void *intal2);
void *karat_brute(void *intal1, void *intal2);


char* intal2str(void* intal){
	INTAL *temp_intal = (INTAL *)intal;
	if (temp_intal == NULL || intal==NULL)
	{
		char *nan = (char*)malloc(4 * sizeof(char));
		nan[0] = 'N';
		nan[1] = 'a';
		nan[2] = 'N';
		nan[3] = '\0';
		return nan;
	}
	char *str = (char *)malloc(sizeof(char)*(temp_intal->n+1));
	int i;
	for (i = 0; i < temp_intal->n; ++i){
		str[i] = '0' + temp_intal->integer[temp_intal->n-i-1];	
	}
	str[i] = '\0';
	return str;
}

void *intal_dup(void *intal){	
	INTAL *temp_intal = (INTAL *)intal;
	if(temp_intal == NULL)return NULL;
	return intal_create(intal2str(temp_intal));
}

void intal_destroy(void* intal){
	INTAL *temp_intal = (INTAL *)intal;
	if(temp_intal == NULL)return;
	free(temp_intal->integer);
	free(temp_intal);
}

void* intal_create(const char* str){
	int flag = 1; int i;
	for(i = 0; i < strlen(str); i++)
		{
		if(str[i] > '9' ||  str[i] < '0'){
			flag=0;
			break;
		}
	}
	if(flag == 1)
		{
		INTAL *intal = (INTAL *)malloc(sizeof(INTAL));
		intal->n = strlen(str);
		intal->integer = (int *)malloc(sizeof(int)*(intal->n));
		
		for (i = 0; i < intal->n; ++i){
			intal->integer[i] = str[intal->n-i-1] - '0';	
		}
		return intal;
	}
	else
	{
		return NULL;
	}
}


void *intal_increment(void* intal){
	INTAL *temp_intal = (INTAL *)intal;
	if(temp_intal == NULL) return NULL;
	int i;
	int flag = 0;
	for (i = 0; i < temp_intal->n; ++i){
		if(temp_intal->integer[i] != 9){
			flag=1;
			break;
		}
	}
	if(flag==1){
		temp_intal->integer[i] = temp_intal->integer[i] + 1;
		i--;
		while(i >= 0){
			temp_intal->integer[i] = 0;
			i--;
		}
	}
	else{	
		temp_intal->n++;
		temp_intal->integer = (int *)realloc(temp_intal->integer, sizeof(int)*(temp_intal->n));
		temp_intal->integer[temp_intal->n-1] = 1;
		int i;
		for (i = 0; i < temp_intal->n-1; ++i){
			temp_intal->integer[i] = 0;
		}
	}
	return(temp_intal);
}

void *intal_decrement(void* intal){
	INTAL *temp_intal = (INTAL *)intal;
	if(temp_intal == NULL) return NULL;
	int i=0;
	int flag = 0;
	for (; i < temp_intal->n ; ++i){
		if(temp_intal->integer[i] != 0){
			flag=1;
			break;
		}
	}
	if(flag==1){	
		temp_intal->integer[i] = temp_intal->integer[i] - 1;
		i--;

		while(i >= 0){
			temp_intal->integer[i] = 9;
			i--;
		}
	}
	else{	
		temp_intal->n = 1;
		temp_intal->integer = (int *)realloc(temp_intal->integer, temp_intal->n);
		temp_intal->integer[i] = 0;
	}
	remove_leading_zeroes(intal);
	return(temp_intal);
	
}

void* intal_add(void* intal1, void* intal2){
	INTAL *temp_intal1 = (INTAL *)intal1;
	INTAL *temp_intal2 = (INTAL *)intal2;
	if(temp_intal1 == NULL || temp_intal2 == NULL)return NULL;
	int n_max = temp_intal1->n > temp_intal2->n ? temp_intal1->n : temp_intal2->n ;
	int n_min = temp_intal1->n < temp_intal2->n ? temp_intal1->n : temp_intal2->n ;
	int carry;
	int res;
	int i;
	INTAL *intal = (INTAL *)malloc(sizeof(INTAL));
	intal->n = n_max+1;
	intal->integer = (int *)malloc(sizeof(int)*(intal->n));
	for (i = 0; i < intal->n; ++i){
		intal->integer[i] = 0;
	}
	for (i = 0; i < n_min; ++i){
		res = temp_intal1->integer[i] + temp_intal2->integer[i] + intal->integer[i];
		carry = res/10;
		res = res%10;
		intal->integer[i] = res;
		intal->integer[i+1] += carry;
	}
	if(temp_intal1->n > temp_intal2->n){
		for (; i < n_max; ++i){
			res = temp_intal1->integer[i] + intal->integer[i];
			carry = res/10;
			res = res%10;
			intal->integer[i] = res;
			intal->integer[i+1] += carry;
		}
	}
	else if(temp_intal1->n < temp_intal2->n){
		for (; i < n_max; ++i){
			res = temp_intal2->integer[i] + intal->integer[i];
			carry = res/10;
			res = res%10;
			intal->integer[i] = res;
			intal->integer[i+1] += carry;
		}
	}
	remove_leading_zeroes(intal);
	return intal;
}


void* intal_diff(void* intal1, void* intal2){
	INTAL *temp_intal1 = (INTAL *)intal1;
	INTAL *temp_intal2 = (INTAL *)intal2;
	if(temp_intal1 == NULL || temp_intal2 == NULL)return NULL;
	INTAL *intal = (INTAL *)malloc(sizeof(INTAL));
	int n_max = temp_intal1->n > temp_intal2->n ? temp_intal1->n : temp_intal2->n ;
	int n_min = temp_intal1->n < temp_intal2->n ? temp_intal1->n : temp_intal2->n ;
	int flag = intal_compare(temp_intal1, temp_intal2);
	int i;
	int k;
	if(flag == 1){ 
		intal->integer = (int *)malloc(sizeof(int)*n_max);
		intal->n = n_max;
		for (i = 0; i < intal->n; ++i){
			intal->integer[i] = temp_intal1->integer[i];
		}
		for(i = 0; i < n_min; i++){
			if(intal->integer[i] >= temp_intal2->integer[i])
				intal->integer[i] = intal->integer[i] - temp_intal2->integer[i];
			else{
				k = i + 1;
				while(intal->integer[k] == 0 && k < n_max)k++;
				intal->integer[k]--;
				k--;
				while(intal->integer[k] == 0 && k > i)intal->integer[k--] = 9;
				intal->integer[i] = intal->integer[i]+10;
				intal->integer[i] = intal->integer[i] - temp_intal2->integer[i];
			}
		}
	}
	else if(flag == -1){
		intal->integer = (int *)malloc(sizeof(int)*n_max);
		intal->n = n_max;
		for (i = 0; i < intal->n; ++i){
			intal->integer[i] = temp_intal2->integer[i];
		}
		for(i = 0; i < n_min; i++){
			if(intal->integer[i] >= temp_intal1->integer[i])
				intal->integer[i] = intal->integer[i] - temp_intal1->integer[i];
			else{
				k = i + 1;
				while(intal->integer[k] == 0 && k < n_max)k++;
				intal->integer[k]--;
				k--;
				while(intal->integer[k] == 0 && k > i)intal->integer[k--] = 9;
				intal->integer[i] = intal->integer[i]+10;
			
				intal->integer[i] = intal->integer[i] - temp_intal1->integer[i];
			}
		}
	}
	else{
		intal->integer = (int *)malloc(sizeof(int)*1);
		intal->n = 1;
		intal->integer[0] = 0;
	}
	remove_leading_zeroes(intal);
	return intal;
}

int intal_compare(void* intal1, void* intal2){
	if(intal1 == NULL || intal2 == NULL)
		return -2;
	INTAL *temp_intal1 = (INTAL *)intal1;
	INTAL *temp_intal2 = (INTAL *)intal2;
	remove_leading_zeroes(intal1);
	remove_leading_zeroes(intal2);
	if(temp_intal1->n > temp_intal2->n)
		return 1;
	else if(temp_intal1->n < temp_intal2->n)
		return -1;
	else{
		int i = temp_intal1->n-1;
		for (; i >= 0; --i){
			if(temp_intal1->integer[i] > temp_intal2->integer[i])
				return 1;
			else if(temp_intal1->integer[i] < temp_intal2->integer[i])
				return -1;
		}
		return 0;
	}
}


void compareable(void *intal1, void *intal2){
	INTAL *temp_intal1 = (INTAL *)intal1;
	INTAL *temp_intal2 = (INTAL *)intal2;
	int n = temp_intal1->n > temp_intal2->n ? temp_intal1->n : temp_intal2->n;
	int i;
	if(temp_intal1->n > temp_intal2->n){
		char *temp = intal2str(intal2);
		temp_intal2->integer = (int *)realloc(temp_intal2->integer, sizeof(int)*n);
		for(i = 0; i < strlen(temp); i++){
			temp_intal2->integer[i] = temp[strlen(temp)-i-1] - '0';
		}
		for(; i < n; i++){
			temp_intal2->integer[i] = 0;
		}
		temp_intal2->n = n;
	}
	else if(temp_intal1->n < temp_intal2->n){
		char *temp = intal2str(temp_intal1);
		temp_intal1->integer = (int *)realloc(temp_intal1->integer, sizeof(int)*n);
		for(i = 0; i < strlen(temp); i++){
			temp_intal1->integer[i] = temp[strlen(temp)-i-1] - '0';
		}
		for(; i < n; i++){
			temp_intal1->integer[i] = 0;
		}
		temp_intal1->n = n;
	}
}


void remove_leading_zeroes(void *intal){	
	INTAL *temp_intal = (INTAL *)intal;
	int i = temp_intal->n-1;
	int j = 0;
	while(temp_intal->integer[i] == 0 && i > 0)--i;
	if(i == temp_intal->n-1)return;
	int *temp = (int *)malloc(sizeof(int)*(i+1));
	for(j = 0; j <= i; j++){
		temp[j] = temp_intal->integer[j];
	}
	free(temp_intal->integer);
	temp_intal->integer = temp;
	temp_intal->n = i+1;
}


void *intal_divide(void *intal1, void *intal2){
	//printf("Somethng");
	if(intal1==NULL || intal2==NULL)
		return NULL;
	char *dividend = intal2str(intal1);
	char *divisor = intal2str(intal2);
	char *quotient = (char *)malloc(sizeof(char)*strlen(divisor)+1);
	INTAL *zero = (INTAL *)intal_create("0");
	if(intal_compare(intal1, intal2) == -1){
		return intal_create("0");
	}
	if(intal_compare(intal2, zero) == 0){
	void* tmp=NULL;
	//intal2str(tmp);
	return tmp;
	}
	char *part = (char *)malloc(sizeof(char)*strlen(dividend)+1);
	int i = strlen(divisor)-1;
	int j;
	int k = 0;
	char *div,*mod;
	INTAL *intal_div;
	INTAL *intal_part;
	INTAL *intal_mod;
	
	for(j = 0; j <= i; j++){
		part[j] = dividend[j];
	}	
	part[j] = '\0';
	while(i < strlen(dividend)){
		intal_part = (INTAL *)intal_create(part);
		remove_leading_zeroes(intal_part);
		intal_div = (INTAL *)intal_mul_divide(intal_part, intal2);
		div = intal2str(intal_div);
		quotient[k++] = div[0];
		intal_mod = (INTAL *)intal_modulo(intal_part, intal2);
		mod = intal2str(intal_mod);
		free(intal_part);
		free(intal_div);
		free(intal_mod);
				
		memset(part, 0, sizeof(char)*strlen(dividend)+1);
		sprintf(part, "%s", mod);
		j = strlen(part);
		part[j] = dividend[++i];
		part[j+1] = '\0';
		free(div);
		free(mod);
	}
	quotient[k] = '\0';
	return intal_create(quotient);	
}


void* karat_brute(void* intal1, void* intal2)
{
	INTAL *temp_intal1 = (INTAL *)intal1;
	INTAL *temp_intal2 = (INTAL *)intal2;
	if(temp_intal1 == NULL || temp_intal2 == NULL)return NULL;
	int n1 = temp_intal1->n;
	int n2 = temp_intal2->n;
	INTAL *intal = (INTAL *)malloc(sizeof(INTAL));
	intal->n = n1 + n2;
	intal->integer = (int *)malloc(sizeof(int)*(intal->n));
	int i;
	int j;
	int k1;
	int k2;
	int sum;
	int carry;
	k1 = k2 =  0;
	for (i = 0; i < n1+n2; ++i)
	{
		intal->integer[i] = 0;
	}
	for (i = 0; i < n1; ++i)
	{
		carry = 0;
		k2 = 0;
		for (j = 0; j < n2; ++j)
		{
			sum = temp_intal1->integer[i]*temp_intal2->integer[j] + intal->integer[k1 + k2] + carry;
			carry = sum/10;
			intal->integer[k1 + k2] = sum % 10;
			k2++;
		}
		if (carry > 0)
		{
			intal->integer[k1+k2] += carry;
		}
		k1++;
	}
	remove_leading_zeroes(intal);
	return intal;
}


void* intal_multiply(void* intal1, void* intal2)
{
	if(intal1 == NULL || intal2 == NULL)return NULL;
	return mul_with_karatsuba(intal1, intal2);
}

void* intal_mul_divide(void* intal1, void* intal2)
{
	INTAL *temp;
	INTAL *count = (INTAL *)intal_create("0");
	if(intal_compare(count, intal2) == 0)return NULL;		
	INTAL *N = (INTAL *)intal_dup(intal1);
	INTAL *D = (INTAL *)intal_dup(intal2);
	if(N == NULL || D == NULL)return NULL;
	while(intal_compare(N, D) == 1 || intal_compare(N, D) == 0){	
		temp = intal_diff(N, D); 
		free(N);
		N = temp;
		intal_increment(count);
	}
	intal_destroy(N);
	intal_destroy(D);
	return count;
}

void *intal_modulo(void *intal1, void *intal2)
{
	char *divisor = intal2str(intal2);
	char *quotient = (char *)malloc(sizeof(char)*strlen(divisor)+1);
	char *dividend = intal2str(intal1);
	INTAL *zero = (INTAL *)intal_create("0");
	if(intal_compare(intal1, intal2) == -1)
		return intal_create(intal2str(intal1));
	if(intal_compare(intal2, zero) == 0)
		return NULL;
	char *part = (char *)malloc(sizeof(char)*strlen(dividend)+1);
	int i = strlen(divisor)-1;
	int j;
	int k = 0;
	char *div,*mod;
	INTAL *intal_div,*intal_part, *intal_mod;
	for(j = 0; j <= i; j++)
	{
		part[j] = dividend[j];
	}	
	part[j] = '\0';
	while(i < strlen(dividend))
	{
		intal_part = (INTAL *)intal_create(part);
		intal_div = (INTAL *)intal_mul_divide(intal_part, intal2);
		div = intal2str(intal_div);
		quotient[k++] = div[0];
		intal_mod = (INTAL *)intal_mul_mod(intal_part, intal2);
		mod = intal2str(intal_mod);
		free(intal_part);free(intal_div);free(intal_mod);				
		memset(part, 0, sizeof(char)*strlen(dividend)+1);
		sprintf(part, "%s", mod);
		j = strlen(part);
		part[j] = dividend[++i];
		part[j+1] = '\0';
		free(div);free(mod);
	}
	quotient[k] = '\0';
	return intal_create(part);	
}

void* intal_mul_mod(void* intal1, void* intal2){
	INTAL *temp;
	INTAL *zero = (INTAL *)intal_create("0");	
	if(intal_compare(zero, intal2) == 0)return NULL;		
	INTAL *N = (INTAL *)intal_dup(intal1);
	INTAL *D = (INTAL *)intal_dup(intal2);

	if(N == NULL || D == NULL)return NULL;
	while(intal_compare(N, D) != -1)
	{	
		temp = intal_diff(N, D); 
		free(N);
		N = temp;
	}
	intal_destroy(zero);
	intal_destroy(D);
	return N;
}

void* intal_pow(void *intal1, void *intal2){
	if(intal1 == NULL || intal2 == NULL)
		return NULL;
	INTAL *temp;
	INTAL *zero = (INTAL *)intal_create("0");
	INTAL *one = (INTAL *)intal_create("1");
	INTAL *two = (INTAL *)intal_create("2");
	INTAL *value,*valueagain;
	remove_leading_zeroes(intal2);
	if(intal_compare(intal2, zero) == 0)
		return one;
	INTAL *intal2by2 = (INTAL *)intal_divide(intal2, two);
	temp = intal_pow(intal1, intal2by2);
	intal_destroy(intal2by2);
	INTAL *mod = intal_modulo(intal2, two);
	if(intal_compare(mod, zero) == 0)
	{
		value = intal_multiply(temp, temp);
		intal_destroy(zero);intal_destroy(one);	intal_destroy(two);
		intal_destroy(temp);intal_destroy(mod);
		return value;
	}
	else
	{
		if(intal_compare(intal1, zero) == 1)
		{
			value = intal_multiply(temp, temp);
			valueagain = intal_multiply(value, intal1);
			intal_destroy(value);intal_destroy(zero);
			intal_destroy(one);intal_destroy(two);
			intal_destroy(temp);
			return valueagain;		
		}
		else
		{
			value = intal_multiply(temp, temp);
			valueagain = intal_divide(value, intal1);
			intal_destroy(value);intal_destroy(zero);intal_destroy(one);
			intal_destroy(two);intal_destroy(temp);intal_destroy(mod);
			return valueagain;			
		}
	}

}


void *intal_lsl(void *intal, void *shift){
	INTAL *temp_intal = (INTAL *)intal;
	INTAL *shift_var = (INTAL *)shift;
	INTAL *res = (INTAL *)malloc(sizeof(INTAL));
	int n;
	int n_shift;
	int i;
	int j;
	n_shift = atoi(intal2str(shift_var));
	n = temp_intal->n + n_shift;
	res->integer = (int *)malloc(sizeof(int)*n);
	for (i = 0; i < n_shift; ++i)		
	{
		res->integer[i] = 0;
	}
	for(j = 0; j < temp_intal->n; j++)
	{
		res->integer[i+j] = temp_intal->integer[j];
	}
	res->n = n;
	return res;
}

void *mul_with_karatsuba(void *intal1, void *intal2){		
	compareable(intal1, intal2);
	INTAL *temp_intal1 = (INTAL *)intal1;
	INTAL *temp_intal2 = (INTAL *)intal2;
	INTAL *A, *B, *C, *D, *AC, *BD, *AD_PLUS_BC, *A_PLUS_B, *C_PLUS_D,*nby2, *N;
	INTAL *exponent;
	INTAL *ten = (INTAL *)intal_create("10");
	INTAL *two = (INTAL *)intal_create("2");
	INTAL *zero = (INTAL *)intal_create("0");
	INTAL *temp, *temp1, *temp2,*PROD;
	int n;
	char n_str[10];
	if(temp_intal1->n == 1 || temp_intal2->n == 1)
	{
		free(ten);
		free(two);
		INTAL *mult = (INTAL *) karat_brute(intal1, intal2);
		return mult;
	}
	else if (intal_compare(zero, intal1) == 0 || intal_compare(zero, intal2) == 0)
	{
		return zero;
	}
	else
	{
		n = temp_intal1->n > temp_intal2->n ? temp_intal1->n : temp_intal2->n;
		sprintf(n_str, "%d", n);
		N = (INTAL *)intal_create(n_str);
		nby2 = (INTAL *)intal_mul_divide(N, two);
		exponent = (INTAL *)intal_lsr(ten, n/2);
		A = (INTAL *)intal_divide(intal1, exponent);
		B = (INTAL *)intal_modulo(intal1, exponent);
		C = (INTAL *)intal_divide(intal2, exponent);
		D = (INTAL *)intal_modulo(intal2, exponent);
		AC = (INTAL *)mul_with_karatsuba(A, C);
		BD = (INTAL *)mul_with_karatsuba(B, D);
		A_PLUS_B = (INTAL *)intal_add(A, B);
		C_PLUS_D = (INTAL *)intal_add(C, D);
		free(A);free(B);free(C);free(D);
		AD_PLUS_BC = (INTAL *)mul_with_karatsuba(A_PLUS_B, C_PLUS_D);
		temp = (INTAL *)intal_diff(AD_PLUS_BC, AC);
		free(AD_PLUS_BC);
		AD_PLUS_BC = (INTAL *)intal_diff(temp, BD);
		free(temp);
		INTAL *nby2_into_2 = (INTAL *)karat_brute(two, nby2);
		temp = intal_lsl(AC, nby2_into_2);	
		temp1 = intal_lsl(AD_PLUS_BC, nby2);
		temp2 = intal_add(temp, temp1);
		PROD = intal_add(temp2, BD);
		free(temp);free(temp1);free(temp2);
		free(nby2);free(nby2_into_2);free(BD);
		free(AC);free(A_PLUS_B);free(C_PLUS_D);
		free(AD_PLUS_BC);free(exponent);free(ten);free(two);
		return PROD;
	}
}

void *intal_lsr(void *intal1, int n){
	INTAL *temp_intal1 = (INTAL *)intal1;
	char *result = (char *)malloc(sizeof(char)*temp_intal1->n + n+1);
	char *string =intal2str(intal1);
	int k, i;
	if(n == 0)	
	{
		INTAL *one = (INTAL *)intal_create("0");
		return one;
	}
	else{
		for(k = 0; k < temp_intal1->n; k++)
		{
			result[k] = string[k];
		}
		for(i = 0; i < n-1; i++)
		{
			result[k++] = '0';
		}
	}
	result[k] = '\0';
	return intal_create(result);
}
