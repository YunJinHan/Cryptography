/**
	@file		rsa.c
	@author		윤 진 한 / 2012036901
	@date		2016.11.29
	@brief		mini RSA implementation code
	@details	컴퓨터 공학과 / 암호학 Project#2
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "rsa.h"

uint p, q, e, d, n;

uint paddingNumber = 0;// padding 값이 붙은 횟수.

/**
	@brief		모듈러 덧셈 연산을 하는 함수.
	@param		uint a		: 피연산자1.
	@param		uint b		: 피연산자2.
	@param		byte op		: +, - 연산자.
	@param		uint mod	: 모듈러 값.
	@return		uint result	: 피연산자의 덧셈에 대한 모듈러 연산 값.
	@todo		모듈러 값과 오버플로우 상황을 고려하여 작성한다.
*/
uint ModAdd(uint a, uint b, byte op, uint mod) {
	uint result = 0;
	if (op == '+'){
		if (a + b <= a || a + b <=b ){
			result = a - ( mod - b );
		} else if (a + b <= mod){
			result = a + b;
		} else {
			result = a + b - mod;
		} // overflow 를 방지하기 위한 modulo 계산.
		return result;
	}
	else {
		if (b > a){
			uint tmp = b - a;
			while (tmp >= mod) tmp -= mod;
			result = mod - tmp;
			return result;
		} else {
			result = a - b;
			while (result >= mod) result -= mod;
			return result;
		}
	}
}

/**
	@brief		모듈러 곱셈 연산을 하는 함수.
	@param		uint x		: 피연산자1.
	@param		uint y		: 피연산자2.
	@param		uint mod	: 모듈러 값.
	@return		uint result	: 피연산자의 곱셈에 대한 모듈러 연산 값.
	@todo		모듈러 값과 오버플로우 상황을 고려하여 작성한다.
*/
uint ModMul(uint x, uint y, uint mod) {
	uint tmp,tmp2;
	uint previous;
	uint result = 0;
	uint cnt = 1;

	while ( x >= 1 ) {

	    tmp = (x & 1); // 2 로 나누었을 때 나머지를 구함. (Binary 변환)

	    if (tmp == 1){

	        if (cnt == 1) {
	     		while ( y >= mod ) y -= mod;
	            result = y;
	        } // 처음 result 값 setting.
	        else {
	            previous = y;
	            tmp2 = cnt;

	            while (tmp2 != 1){
	                if (previous + previous < previous){
	                    previous = previous - ( mod - previous );
	                } else if (previous + previous <= mod){
	                    previous = previous + previous;
	                } else {
	                    previous = ModAdd(previous,previous,'+',mod);
	                }
	                // overflow 를 방지하기 위한 modulo 계산.
	                tmp2 >>= 1;
	            }
	            if (result + previous <= result){
	                result = result - ( mod - previous );
	            } else if (result + previous <= mod){
	                result = result + previous;
	            } else {
	                result = ModAdd(result,previous,'+',mod);
	            }
	            // overflow 를 방지하기 위한 modulo 계산.
	        }
	    }
	    x >>= 1;
		cnt <<= 1;
	} // 해당 x 를 Binary 로 바꾸어 x * y 곱셈 연산을 덧셈 연산으로 바꾸어 계산해줌.
	return result;
}

/**
	@brief		모듈러 거듭제곱 연산을 하는 함수.
	@param		uint base	: 피연산자1.
	@param		uint exp	: 피연산자2.
	@param		uint mod	: 모듈러 값.
	@return		uint result	: 피연산자의 연산에 대한 모듈러 연산 값.
	@todo		모듈러 값과 오버플로우 상황을 고려하여 작성한다.
				'square and multiply' 알고리즘을 사용하여 작성한다.
*/
uint ModPow(uint base, uint exp, uint mod) {
	uint result = 1;
	uint tmp = base;

	for ( ; ; exp >>= 1) {
		if (exp <= 0){
			while (result >= mod) result -= mod;
			return result;
		}
		if ((exp & 1) == 1){
			result = ModMul(result,tmp,mod);
		}
		tmp = ModMul(tmp,tmp,mod);
	} // exp 를 절반으로 줄여 나가면서 거듭제곱 연산을 ModMul 을 통하여 overflow 발생 시 해당 overflow 를 처리하며 계산함.
}

/**
	@brief		입력된 수가 소수인지 입력된 횟수만큼 반복하여 검증하는 함수.
	@param		uint testNum	: 임의 생성된 홀수.
	@param		uint repeat		: 판단함수의 반복횟수.
	@return		uint result		: 판단 결과에 따른 TRUE, FALSE 값.
	@todo		Miller-Rabin 소수 판별법과 같은 확률적인 방법을 사용하여,
				이론적으로 4N(99.99%) 이상 되는 값을 선택하도록 한다.
*/
bool IsPrime(uint testNum, uint repeat) {
	uint result = testNum - 1;
	uint randomNum,tmp,modResult;

	if (( testNum != 2 && (testNum & 1) == 0) || (testNum < 2)) return FALSE;

	while ( (result & 1) == 0) result >>= 1;

	while (repeat--){

		tmp = result;
		randomNum = (uint)((double)WELLRNG512a())*testNum + 1;
		while (randomNum >= (testNum - 1))  randomNum -= (testNum - 1);
		randomNum += 1;
		modResult = ModPow(randomNum,tmp,testNum);

		while (tmp != testNum - 1 && modResult != testNum - 1 && modResult != 1){
			modResult = ModMul(modResult, modResult, testNum);
			tmp <<= 1;
		}
		if (modResult != testNum - 1 && (tmp & 1) == 0) return FALSE;
	} // Miller-Rabin 소수 판별 알고리즘을 사용하여 10번 통과시 소수일 확률이 99.99% 이 된다.
	return TRUE;
}

uint GCD(uint a, uint b) {
	uint prev_a;
	while(b != 0) {
		printf("GCD(%u, %u)\n", a, b);
		prev_a = a;
		a = b;
		while(prev_a >= b) prev_a -= b;
		b = prev_a;
	}
	printf("GCD(%u, %u)\n\n", a, b);
	return a;
}

/**
	@brief		모듈러 역 값을 계산하는 함수.
	@param		uint a	: 피연산자1.
	@param		uint m	: 모듈러 값.
	@return		uint result	: 피연산자의 모듈러 역수 값.
	@todo		확장 유클리드 알고리즘을 사용하여 작성하도록 한다.
*/
uint ModInv(uint a, uint m) {
	uint x,x1,x2;
	uint quotient;
	uint t,t1 = 0;
	uint tmp1,tmp2,quotientTmp;
	int result = 1;

	for (x1 = a, x2 = m; x1 != 1; x2 = x1, x1 = x){
		tmp1 = x2;
		tmp2 = x1;
		quotientTmp = 0;
		while (tmp1 >= tmp2){
			quotientTmp ++;
			tmp1 -= tmp2;
		} // 몫을 구함.
		quotient = quotientTmp;

		x = x2 - x1*quotient;
		t = t1 - result*quotient;
		t1 = (uint)result;
		result = t;
	}
	if (result < 0) result += m;
	// 확장 유클리드 알고리즘을 사용하여 해당 a 의 곱셈 역원을 구함.
	return (uint)result;
}

/**
	@brief		RSA 키를 생성하는 함수.
	@param		uint *p	: 소수 p.
	@param		uint *q	: 소수 q.
	@param		uint *e	: 공개키 값.
	@param		uint *d	: 개인키 값.
	@param		uint *n	: 모듈러 n 값.
	@return		void
	@todo		과제 안내 문서의 제한사항을 참고하여 작성한다.
*/
void MRSAKeygen(uint *p, uint *q, uint *e, uint *d, uint *n) {

	while (TRUE){
		while (TRUE){
		    *p = (uint)((double)WELLRNG512a()*(65536 - 46340 + 1)) + 46340;
		    printf("random-number1 %d selected.\n",*p);
			if (IsPrime(*p,10)) {
				printf("%d may be Prime.\n\n",*p);
				break;
			}
			printf("%d is not Prime.\n\n",*p);
		}// p 생성.
		while (TRUE){
			*q = (uint)((double)WELLRNG512a()*(65536 - 46340 + 1)) + 46340;
			printf("random-number2 %d selected.\n",*q);
			if (*q == *p) continue;
			 if (IsPrime(*q,10)) {
				printf("%d may be Prime.\n\n",*q);
				break;
			}
			printf("%d is not Prime.\n\n",*q);
		}// q 생성.
		*n = (*p)*(*q);
		if (*n >= 2147483648 && *n <= 4294967295){
			printf("finally selected prime p, q = %u %u.\nthus, n = %u\n\n",*p,*q,*n);
			uint pi_n = (*p - 1)*(*q - 1);
			while (TRUE){
				*e = ((uint)((double)WELLRNG512a() * (pi_n  - 1) + 2));
				printf("e : %u selected.\n",*e);
				if (GCD(*e,pi_n) == 1) break;
			}// 1 < e < pi_n , gcd(e,pin) == 1 인 e 를 구함.
			*d = ModInv(*e,pi_n);
			printf("d : %u selected.\n\n",*d);
			printf("e d n pi_n : %u %u %u %u\n",*e,*d,*n,pi_n);
			printf("e*d mod pi_n : %u\n\n",ModMul(*e,*d,pi_n));
			break;
		}// p 와 q로 만들어진 n이 32bit 일 경우.
		printf("=========================== < n is not 32bit > ===========================\n\n");
	}
}

/**
	@brief		RSA 암복호화를 진행하는 함수.
	@param		FILE *ifp	: 입력 파일 포인터.
	@param		uint len	: 입력 파일 길이.
	@param		FILE *ofp	: 출력 파일 포인터.
	@param		uint key	: 키 값.
	@param		uint n		: 모듈러 n 값.
	@return		uint result	: 암복호화에 성공한 byte 수.
	@todo		과제 안내 문서의 제한사항을 참고하여 작성한다.
*/

uint FindNBit(uint x ){
  uint ans = 0 ;
  while( x>>=1 ) ans++;
  return ans + 1;
}// 해당 x 가 몇 비트인지 Return 해줌.

uint MRSACipher(FILE *ifp, uint len, FILE *ofp, uint key, uint n) {
	uint tmpText = 0;
	uint result = 0;// 암복호화에 성공한 byte 수.
	uint modBit = FindNBit(n); // mod 의 bit 를 구함.
	uint paddingCnt = (((len&3) == 0) ? 0 : (4 - (len&3)) );
	// DataBlock 단위 (4byte) 로 나뉘지 않을 경우 paddingCnt 길이 만큼 padding(0) 처리함.
	len += paddingCnt; // 입력 파일 길이에 paddingCnt 한 만큼 더해줌.

	char *byte_of_file_data = (char *)malloc(sizeof(char)*(len));
	char *bit_of_file_data = (char *)malloc(sizeof(char)*(len*8));
	char *buffer = (char *)malloc(sizeof(char)*4);
	bzero(byte_of_file_data,len);
	bzero(bit_of_file_data,len*8);
	// Encryption OR Decryption 연산시 필요한 Array 선언 및 초기화 시켜줌.

	fread(byte_of_file_data,len - paddingCnt,1,ifp);
	// input file pointer 로 부터 data 를 읽어옴. 

	if (paddingCnt) {
		paddingNumber = paddingCnt;
		for (uint i = len - 1; i >= len - paddingCnt; i --){
			byte_of_file_data[i] = '0';
		}
	}// 모자란 공간을 padding 으로 채워줌.

	/* String_To_Bit */
	for (uint i = 0; i < len; i ++){
		for (uint j = 8; j >= 1; j --){
			*(bit_of_file_data + ( i * 8 ) + ( 8 - j )) = (( *(byte_of_file_data + i) & (0x01 << ( j - 1 ))) >> ( j - 1 ));
		}
	}// 해당 text 를 전부 bit 로 변환시켜줌.
	printf("MRSACipher start. file len is %u\n\n",len-paddingCnt);

	for (uint i = 0; i < (len>>2); i ++){

		tmpText = 0;
		bzero(buffer,4);

		printf("len : %u\n",len-paddingCnt-i*4);
		for (uint j = 0; j < 4; j ++){
			*(buffer + j) = *(byte_of_file_data + i*4 + j );
		}
		printf("buf : %s\n",buffer);// 4byte 씩 끊어서 출력해줌.

		/* Bit_To_Uint */
		for (uint j = 0; j < modBit; j ++){
			tmpText |= (*(bit_of_file_data + (i*modBit) + j) << (modBit - j - 1));
		}
		printf("ptx : %u\n",tmpText);// 32bit 씩 Uint type 으로 변형 시켜줌.

		if (tmpText > n) {
			printf("Error Encryption / Decryption to overflow\n");
		}// 블록 단위로 조각난 데이터의 값이 모듈러 n 값 보다 큰 경우 -> 오류처리.

		/* Encryption OR Decryption */
		tmpText = ModPow(tmpText,key,n);
		printf("ctx : %u\n",tmpText);

		/* Uint_To_Bit */
		for (uint j = 0; j < modBit; j ++){
			*(bit_of_file_data + (i*modBit) + j) = (tmpText & (0x01 << (modBit - j - 1))) >> (modBit - j - 1);
		}// Encryption OR Decryption 이 수행된 Uint type 의 값을 다시 bit 형태로 바꾸어줌.

		result += 4; // Encryption OR Decryption 에 성공한 byte 수를 더해줌.
		putchar('\n');
	}// 해당 file text 를 4byte 씩 끊어 연산을 수행함.
	bzero(byte_of_file_data,len); // Encryption OR Decryption 하기 전의 byte 배열을 초기화시킴. 
	
	/* Bit_To_String */
	for (uint i = 0; i < len; i ++){
		for (uint j = 8; j >= 1; j --){
			*(byte_of_file_data + i) |= ( *(bit_of_file_data + ( i * 8 ) + ( 8 - j )) << ( j - 1 ));
		}
	}// 모든 text 의 Encryption OR Decryption 수행이 완료되면 해당 결과 bit 들을 byte 로 변환시켜 저장함.

	fwrite(byte_of_file_data,1,len,ofp);
	// output file pointer 로 Encryption OR Decryption 이 수행된 결과를 write 함. 

	free(buffer);
	free(bit_of_file_data);
	free(byte_of_file_data);
	// malloc 으로 선언한 배열들을 free 시켜줌.
	return result;
}

void deletePadding(FILE *ofp,uint fsize){
	fseek(ofp,fsize-paddingNumber,SEEK_SET);
	while (paddingNumber--){
		fwrite(" ",1,1,ofp);
	}
	// padding 값으로 추가된 "0" 값을 삭제해줌.
}

int main(int argc, char const *argv[]) {
	uint seed = time(NULL);
	InitWELLRNG512a(&seed);
	
	FILE *data_fp, *enc_fp, *dec_fp;
	uint fsize;
	
	if(argc != 4) {
		printf("usage : ./rsa data_file encrypt_file decrypt_file\n");
		exit(1);
	}

	data_fp = fopen(argv[1], "rb");
	enc_fp = fopen(argv[2], "wb");
	if(data_fp == NULL | enc_fp == NULL) {
		printf("file open fail\n");
		exit(1);
	}

	fseek(data_fp, 0, SEEK_END);
	fsize = ftell(data_fp);
	printf("data file size : %u\n\n", fsize);
	fseek(data_fp, 0, SEEK_SET);
	
	MRSAKeygen(&p, &q, &e, &d, &n);
	
	fsize = MRSACipher(data_fp, fsize, enc_fp, e, n);

	fclose(data_fp);
	fclose(enc_fp);
	
	enc_fp = fopen(argv[2], "rb");
	dec_fp = fopen(argv[3], "wb");
	if(dec_fp == NULL | enc_fp == NULL) {
		printf("file open fail\n");
		exit(1);
	}

	printf("encrypted file size : %u\n\n", fsize);

	fsize = MRSACipher(enc_fp, fsize, dec_fp, d, n);

	deletePadding(dec_fp,fsize);
	// padding 값으로 추가된 "0" 값을 삭제해줌.
	
	fclose(enc_fp);
	fclose(dec_fp);
	
	return 0;
}