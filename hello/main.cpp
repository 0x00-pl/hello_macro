#include<iostream>
using namespace std;
int prtf= ((int)(void*)&printf);
int _m[]={0x8bec8b55, 0x088b0c45, 0xff519090, prtf, 0x3304c483, 0x90c35dc0};
typedef int maintype(int,char**);

int main2(int argc, char** argv){}


int main(int argc, char** argv){
}


#if 0
int main(int argc, char* argv[]){
00401000 55                   push        ebp  
00401001 8B EC                mov         ebp,esp  
	printf(argv[0]);
00401003 8B 45 0C             mov         eax,dword ptr [argv]  
00401006 8B 08                mov         ecx,dword ptr [eax]  
00401008 51                   push        ecx  
00401009 FF 15 A0 20 40 00    call        dword ptr [__imp__printf (4020A0h)]  
0040100F 83 C4 04             add         esp,4  
}
00401012 33 C0                xor         eax,eax  
00401014 5D                   pop         ebp  
00401015 C3                   ret  

#endif