#include <iostream>
#include <fstream>
#include <bitset>
#include <string>
#include <iomanip>
#include <string.h>
using namespace std;


int tosign(long int k, string str){
	
	bitset<32> bits(str);
        long int imm=bits.to_ulong();
        if(imm>=k)
        	imm=imm-k*2;
	return imm;
	}
void error(){cerr<<"error"<<endl;
exit(-1);
}
	
string getrs1(bitset<8> inst2,bitset<8> inst3){
string rs1=inst2.to_string().erase(0,4);
rs1+=char('0'+inst3[7]);
return rs1;
}

string getrs2(bitset<8> inst1,bitset<8> inst2){
string rs2=char(inst1[0]+'0')+inst2.to_string().erase(4,4);
return rs2;
}

string getfunc3(bitset<8> inst3){
string func3=inst3.to_string().erase(4,4).erase(0,1);
return func3;
}

string getfunc7(bitset<8> inst1){
string func7=inst1.to_string().erase(7,1);
return func7;
}

string getrd(bitset<8> inst3,bitset<8> inst4){
string rd=inst3.to_string().erase(0,4);
rd+=char('0'+inst4[7]);
return rd;
}

string getinst(bitset<8> inst1,bitset<8> inst2,bitset<8> inst3, bitset<8> inst4){
string inst=inst1.to_string()+inst2.to_string()+inst3.to_string()+inst4.to_string();
return inst;
}
void execute(char* name, void * pdata, int N){
ifstream input(name,ios::binary);
if(!input){
	error();}
char temp[4];
	int num=0;
	int X[33]={0,};
	int maxread=0;
	input.read(temp,4);
	while(1){
	
	maxread++;
	if(input.eof()||maxread>N){break;}
	bitset<8> inst1=temp[3];
	bitset<8> inst2=temp[2];
	bitset<8> inst3=temp[1];
	bitset<8> inst4=temp[0];
	string opcode=inst4.to_string().erase(0,1);
	string rs2=getrs2(inst1,inst2);
	string rs1=getrs1(inst2,inst3);
	string func3=getfunc3(inst3);
	string func7=getfunc7(inst1);
	string rd=getrd(inst3,inst4);
	string inst=getinst(inst1, inst2,inst3,inst4);
	bitset<32> bits(inst);
	unsigned int bitinst=bits.to_ulong();

	unsigned short srs1=tosign(40,rs1), srs2=tosign(40,rs2),srd=tosign(40,rd);
	if(!srd){
	srd=32;}
	
		if(opcode=="0110111"||opcode=="0010111"){
		int imm=tosign(2147483648,func7+rs2+rs1+func3+"000000000000");
		
		if(opcode=="0110111"){//lui
			X[srd]=imm;}
		else{//auipc
			X[srd]=input.tellg();
			X[srd]+=imm-4;
		}
		
	}
	else if(opcode=="1101111"){//jal
		string str=rs1+func3;
		str.append(1,rs2[4]);
		str.insert(0,1,func7[0]);
		str.append(func7+rs2);
		str.erase(10,1);
		str.pop_back();
	
		int imm=tosign(1048576,str+"0");
	
		X[srd]=input.tellg();
		input.seekg(imm-4, std::ios::cur);
		
        }
	else if(opcode=="1100111"&&func3=="000"){//jalr
		int imm=tosign(2048,func7+rs2);
	
		X[srd]=input.tellg();
		input.seekg(imm+X[srs1], std::ios::beg);
        }
	else if(opcode=="1100011"){//b
		string str=func7+rd;
		str.insert(1,1,str[11]);
		str.pop_back();
		bool istrue=false;
		int imm=tosign(4096,str+"0");
		if(func3=="000"){//beq
                        if(X[srs1]==X[srs2]){ input.seekg(imm-4, std::ios::cur);
			}}
                else if(func3=="001"){//bne
                        if(X[srs1]!=X[srs2]){ input.seekg(imm-4, std::ios::cur);
				
			}}
                else if(func3=="100"){//blt
                        if(X[srs1]<X[srs2]){ input.seekg(imm-4, std::ios::cur);
			}}
                else if(func3=="101"){//bge
                        if(X[srs1]>=X[srs2]){ input.seekg(imm-4, std::ios::cur);
			}}
                else if(func3=="110"){//bltu
                        if((unsigned int)X[srs1]<(unsigned int)X[srs2]){ input.seekg(imm-4, std::ios::cur);
			}}
		else if(func3=="111"){//bgeu
			if((unsigned int)X[srs1]>=(unsigned int)X[srs2]){ input.seekg(imm-4, std::ios::cur);
			}}

                else{cout<<"unknown instruction"<<endl;
                        continue;}
                


        }
	else if(opcode=="0000011"){//l
				   
		int imm=tosign(2048,func7+rs2);
		
                if(func3=="000"){//lb
                        X[srd]=*(((char*)pdata+(X[srs1]-0x10000000+imm)));}
                else if(func3=="001"){//lh
                        X[srd]=*((short*)((char*)pdata+(X[srs1]-0x10000000+imm)));}
                else if(func3=="010"){//lw
			if(X[srs1]+imm==0x20000000){
                        cin>>X[srd];}
                        else
                        X[srd]=*((int*)((char*)pdata+(X[srs1]-0x10000000+imm)));}
		else if(func3=="100"){//lbu
				      
			X[srd]=*((char*)((char*)pdata+(X[srs1]-0x10000000+imm)));
			if(X[srd]<0) X[srd]=256+X[srd];	
		}
		else if(func3=="101"){//lhu
				     
			X[srd]=*((short*)((char*)pdata+(X[srs1]-0x10000000+imm)));
			if(X[srd]<0) X[srd]+=65536;
		}
                else{cout<<"unknown instruction"<<endl;
                        continue;}
                
        
        }
	else if(opcode=="0100011"){//s
				   
		int imm=tosign(2048,func7+rd);
		if(func3=="000"){//sb
			*(((char*)pdata+(X[srs1]-0x10000000+imm)))=X[srs2]%256;}
		else if(func3=="001"){//sh
			*(((short*)(char*)pdata+(X[srs1]-0x10000000+imm)))=X[srs2]%65536;}
		else if(func3=="010"){//sw
			if(X[srs1]+imm==0x20000000) cout<<char(X[srs2]);
			else
				*((int*)((char*)pdata+(X[srs1]-0x10000000+imm)))=X[srs2];}
		else{cout<<"unknown instruction"<<endl;
                        continue;}
		
        }
	else if(opcode=="0010011"){//ari i
		int imm=tosign(2048,func7+rs2);
		if(func3=="000")//addi
			X[srd]=X[srs1]+imm;
		else if(func3=="010")//slti
			X[srd]=(X[srs1]<imm?1:0); 
		else if(func3=="011")//sltiu
			X[srd]=((unsigned int)X[srs1]<(unsigned int)imm?1:0);
		else if(func3=="100")//xori
			X[srd]=X[srs1]^imm;
		else if(func3=="110")//ori
			X[srd]=X[srs1]|imm;
		else if(func3=="111")//andi
			X[srd]=X[srs1]&imm;
		else if(func3=="001"||func3=="101"){
			imm=srs2;
			if(func3=="001"&&func7=="0000000")//slli
				X[srd]=X[srs1]<<imm;
			else if(func3=="101"&&func7=="0000000")//srli
				X[srd]=(unsigned int)X[srs1]>>imm;
			else if(func3=="101"&&func7=="0100000")//srai
				X[srd]=X[srs1]>>imm;
			else{
			cout<<"unknown instruction"<<endl;
			continue;}
		}
		else{cout<<"unknown instruction"<<endl;
                        continue;}

        }
	else if(opcode=="0110011"){
	if(func3=="000"){
		if(func7=="0000000")//add
			X[srd]=X[srs1]+X[srs2];
		else if(func7=="0100000")//sub
			X[srd]=X[srs1]-X[srs2];
		else{cout<<"unknown instruction"<<endl;
		continue;}
	}
	else if(func3=="001"&&func7=="0000000"){
		//sll
		X[srd]=X[srs1]<<X[srs2];}
	else if(func3=="010"&&func7=="0000000"){//slt
		X[srd]=(X[srs1]<X[srs2]?1:0);}
        else if(func3=="011"&&func7=="0000000"){//sltu
		X[srd]=((unsigned int)X[srs1]<(unsigned int)X[srs2]?1:0);}
        else if(func3=="100"&&func7=="0000000"){//xor
		X[srd]=X[srs1]^X[srs2];}
        else if(func3=="101"&&func7=="0000000"){//srl
		X[srd]=(unsigned int)(X[srs1])>>X[srs2];}
        else if(func3=="101"&&func7=="0100000"){//sra
		X[srd]=X[srs1]>>X[srs2];}
        else if(func3=="110"&&func7=="0000000"){//or
		X[srd]=X[srs1]|X[srs2];}
        else if(func3=="111"&&func7=="0000000"){//and
		X[srd]=X[srs1]&X[srs2];}
	else{
	cout<<"unknown instruction"<<endl;
	continue;}
	
	}
	else{
	cout<<"unknown instruction"<<endl;
	continue;}
	
	input.read(temp,4);
	
	}
	for(int i=0;i<32;i++){
		cout<<'x'<<dec<<i<<": 0x"<<hex<<setw(8)<<setfill('0')<<X[i]<<endl;
			
	}
}

int main(int argc, char* argv[]){
	
	ifstream data;
	void * pdata=malloc(65536);
	memset(pdata,-1,65536);
	
	int N=0;
	if(argc==3){
		string n(argv[2]);
	N=stoi(n);
	
	}
	else if(argc==4){string n(argv[3]);
		N=stoi(n);
		data.open(argv[2]);
		data.read((char*)(pdata),65536);
	}
	else{
	error();}
	
	
	execute(argv[1],pdata,N);
}
