#include <iostream>
#include <fstream>
#include <bitset>
#include <string>
#include <iomanip>
#include <string.h>

using namespace std;


int tosign(long int k, int imm){
	
        
        if(imm>=k)
        	imm=imm-k*2;
	return imm;
	}
	
	
void error(){cerr<<"error"<<endl;
exit(-1);
}
	
unsigned short getrs1(unsigned int inst){
unsigned short rs1=inst%(2<<19)/(2<<14);
return rs1;
}

unsigned short getrs2(unsigned int inst){
unsigned short rs2=inst%(2<<24)/(2<<19);
return rs2;
}

unsigned short getfunc3(unsigned int inst){
unsigned short func3=inst%(2<<14)/(2<<11);
return func3;
}

unsigned short getfunc7(unsigned int inst){
unsigned short func7=inst>>25;
return func7;
}

unsigned short getrd(unsigned int inst){
unsigned short rd=inst%(2<<11)/(2<<6);
return rd;
}
unsigned int getinst(char* temp){

unsigned int inst=((unsigned char)(temp[0]));

inst+=((unsigned char)(temp[1]))*(2<<7);

inst+=((unsigned char)(temp[2]))*(2<<15);
inst+=(unsigned char)(temp[3])*(2<<23);

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
	unsigned int inst=getinst(temp);
	unsigned int opcode=inst%(2<<6);
	unsigned int rs2=getrs2(inst);
	unsigned int rs1=getrs1(inst);
	unsigned int func3=getfunc3(inst);
	unsigned int func7=getfunc7(inst);
	unsigned int rd=getrd(inst);
		
	if(!rd){
	rd=32;}
	
        if(opcode==55||opcode==23){
		int imm=(inst&0xFFFFF000);
		
		if(opcode==55){//lui
			X[rd]=imm;}
		else{//auipc
			X[rd]=input.tellg();
			X[rd]+=imm-4;
		}
		
	}
	else if(opcode==111){//jal
		
	int imm=((inst&0x80000000)?0xFFF00000:0)|((inst>>20)&0x7FE)|((inst>>9)&0x800)|((inst>>0)&0xFF000);
	
		X[rd]=input.tellg();
		input.seekg(imm-4, std::ios::cur);
		
        }
	else if(opcode==103&&func3==0){//jalr
		int imm=(func7*(0x10)+rs2);

		X[rd]=input.tellg();
		input.seekg(imm+X[rs1], std::ios::beg);
        }
	else if(opcode==99){//b
		
		int imm=((inst >>31)&0x1)<<12|((inst>>25)&0x3F)<<5|((inst>>8)&0xF)<<1|((inst>>7)&0x1)<<11;
		imm=tosign(4096,imm);
		
		if(func3==0){//beq
                        if(X[rs1]==X[rs2]){ input.seekg(imm-4, std::ios::cur);
			}}
                else if(func3==1){//bne
                        if(X[rs1]!=X[rs2]){ input.seekg(imm-4, std::ios::cur);
				
			}}
                else if(func3==4){//blt
                        if(X[rs1]<X[rs2]){ input.seekg(imm-4, std::ios::cur);
			}}
                else if(func3==5){//bge
                        if(X[rs1]>=X[rs2]){ input.seekg(imm-4, std::ios::cur);
			}}
                else if(func3==6){//bltu
                        if((unsigned int)X[rs1]<(unsigned int)X[rs2]){ input.seekg(imm-4, std::ios::cur);
			}}
		else if(func3==7){//bgeu
			if((unsigned int)X[rs1]>=(unsigned int)X[rs2]){ input.seekg(imm-4, std::ios::cur);
			}}

                else{cout<<"unknown instruction"<<endl;
                        continue;}
                


        }
	else if(opcode==3){//l
				   
		int imm=((inst>>20)&0xFFF);
		imm=tosign(2048,imm);
		
                if(func3==0){//lb
                        X[rd]=*(((char*)pdata+(X[rs1]-0x10000000+imm)));}
                else if(func3==1){//lh
                        X[rd]=*((short*)((char*)pdata+(X[rs1]-0x10000000+imm)));}
                else if(func3==2){//lw
			if(X[rs1]+imm==0x20000000){
                        cin>>X[rd];}
                        else
                        X[rd]=*((int*)((char*)pdata+(X[rs1]-0x10000000+imm)));}
		else if(func3==4){//lbu
				      
			X[rd]=*((char*)((char*)pdata+(X[rs1]-0x10000000+imm)));
			if(X[rd]<0) X[rd]=256+X[rd];	
		}
		else if(func3==5){//lhu
				     
			X[rd]=*((short*)((char*)pdata+(X[rs1]-0x10000000+imm)));
			if(X[rd]<0) X[rd]+=65536;
		}
                else{cout<<"unknown instruction"<<endl;
                        continue;}
                
        
        }
	else if(opcode==35){//s
				   if(rd==32) rd=0;
		int imm=(func7*32+rd);
		imm=tosign(2048,imm);
		
		if(func3==0){//sb
			*(((char*)pdata+(X[rs1]-0x10000000+imm)))=X[rs2]%256;}
		else if(func3==1){//sh
			*(((short*)(char*)pdata+(X[rs1]-0x10000000+imm)))=X[rs2]%65536;}
		else if(func3==2){//sw
			if(X[rs1]+imm==0x20000000) cout<<char(X[rs2]);
			else
				*((int*)((char*)pdata+(X[rs1]-0x10000000+imm)))=X[rs2];}
		else{cout<<"unknown instruction"<<endl;
                        continue;}
		
        }
	else if(opcode==19){//ari i
		int imm=((inst>>20)&0xFFF);
		imm=tosign(2048,imm);
		if(func3==0)//addi
			X[rd]=X[rs1]+imm;
		else if(func3==2)//slti
			X[rd]=(X[rs1]<imm?1:0); 
		else if(func3==3)//sltiu
			X[rd]=((unsigned int)X[rs1]<(unsigned int)imm?1:0);
		else if(func3==4)//xori
			X[rd]=X[rs1]^imm;
		else if(func3==6)//ori
			X[rd]=X[rs1]|imm;
		else if(func3==7)//andi
			X[rd]=X[rs1]&imm;
		else if(func3==1||func3==5){
			imm=rs2;
			if(func3==1&&func7==0)//slli
				X[rd]=X[rs1]<<imm;
			else if(func3==5&&func7==0)//srli
				X[rd]=(unsigned int)X[rs1]>>imm;
			else if(func3==5&&func7==32)//srai
				X[rd]=X[rs1]>>imm;
			else{
			cout<<"unknown instruction"<<endl;
			continue;}
		}
		else{cout<<"unknown instruction"<<endl;
                        continue;}

        }
	else if(opcode==51){
	if(func3==0){
		if(func7==0)//add
			X[rd]=X[rs1]+X[rs2];
		else if(func7==32)//sub
			X[rd]=X[rs1]-X[rs2];
		else{cout<<"unknown instruction"<<endl;
		continue;}
	}
	else if(func3==1&&func7==0){
		//sll
		X[rd]=X[rs1]<<X[rs2];}
	else if(func3==2&&func7==0){//slt
		X[rd]=(X[rs1]<X[rs2]?1:0);}
        else if(func3==3&&func7==0){//sltu
		X[rd]=((unsigned int)X[rs1]<(unsigned int)X[rs2]?1:0);}
        else if(func3==4&&func7==0){//xor
		X[rd]=X[rs1]^X[rs2];}
        else if(func3==5&&func7==0){//srl
		X[rd]=(unsigned int)(X[rs1])>>X[rs2];}
        else if(func3==5&&func7==32){//sra
		X[rd]=X[rs1]>>X[rs2];}
        else if(func3==6&&func7==0){//or
		X[rd]=X[rs1]|X[rs2];}
        else if(func3==7&&func7==0){//and
		X[rd]=X[rs1]&X[rs2];}
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
