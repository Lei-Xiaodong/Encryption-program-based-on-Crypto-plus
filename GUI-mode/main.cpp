#include <stdio.h>
#include <iostream>
#include <string>
#include <randpool.h>
#include <rsa.h>
#include <hex.h>
#include <aes.h>
#include <des.h>
#include <md5.h>
#include <files.h>
#pragma comment( lib, "cryptlib.lib" )
using namespace std; 
using namespace CryptoPP;

void myDES(FILE *fin,FILE *fout,bool type); //DES�㷨
string Hash(char path[],char hash[]); //MD5�㷨
void mySign(char path[],FILE *fin,FILE *fout,bool type); //����ǩ��
//--------rsa�ĺ���-----------//
void GenerateRSAKey(unsigned int keyLength, const char *privFilename, const char *pubFilename, const char *seed);
string RSAEncryptString(const char *pubFilename, const char *seed, const string message);
string RSADecryptString(const char *privFilename, const string ciphertext);
RandomPool & GlobalRNG()
{	static RandomPool randomPool;
	return randomPool;
}
//--------rsa�ĺ���-----------//
void putkey(FILE *fin,FILE *fout,bool type); //�����ŷ�+����ǩ��(������Կ)

int main(int argc,char *argv[])
{	string option=argv[1];
	FILE *fin,*fout;
	fin = fopen(argv[2],"rb");
	
	if(option=="des1") //des����
	{	fout = fopen(argv[3],"wb");
		myDES(fin,fout,1);
		cout<<"�ɹ���";
	}
	else if(option=="des0") //des����
	{	fout = fopen(argv[3],"wb");
		myDES(fin,fout,0);
		cout<<"�ɹ���";
	}
	else if(option=="md5a") //����hash
	{	fout = fopen(argv[3],"wb");
		char hash[33]={'\0'};
		Hash(argv[2],hash);
		fwrite(hash,32,1,fout);
		cout<<hash<<endl;  
	}
	else if(option=="md5b") //�Ƚ�hash
	{	char ahash[33]={'\0'},bhash[33]={'\0'};
		fout = fopen(argv[3],"rb");
		fread(ahash,32,1,fin);
		fread(bhash,32,1,fout);
		cout<<ahash<<" "<<bhash<<endl;
		if(string(ahash)!=string(bhash)) cout<<"ժҪ��һ�£�";
		else cout<<"ժҪһ�£�";
	}
	else if(option=="sign1")
	{	fout = fopen(argv[3],"wb");
		mySign(argv[2],fin,fout,1);
	}
	else if(option=="sign0")
	{	fout = fopen(argv[3],"wb");
		mySign(argv[3],fin,fout,0);
	}
	else if(option=="alluse1")
	{	fout = fopen(argv[3],"wb");
		mySign(argv[2],fin,fout,1);
		fclose(fout);fclose(fin);
		fout=fopen(argv[3],"rb");
		fin =fopen(argv[2],"wb");
		myDES(fout,fin,1);
		putkey(fin,fout,1);
		cout<<argv[2]<<"�Ѽ���\n";
		cout<<"���ܺ�Ĺ�Կ����key.txt\n";
	}
	else if(option=="alluse0")
	{	fout = fopen(argv[3],"wb");
		putkey(fin,fout,0);
		myDES(fin,fout,0);
		fclose(fout);fclose(fin);
		fout=fopen(argv[3],"rb");
		fin =fopen(argv[2],"wb");
		mySign(argv[2],fout,fin,0);
		cout<<argv[2]<<"�ѽ���\n";
		cout<<"���ܺ�Ĺ�Կ����key.txt\n";
	}
	else
		cout<<"��Ч������";
	fclose(fin);
	fclose(fout);
	return 0;
}
//DES
void myDES(FILE *fin,FILE *fout,bool type) //DES
{	unsigned char key[8]={89,146,116,197,155,228,172,93};
    unsigned char input[8];
    unsigned char output[8];
	
	if(type) //����
	{	bool NotEnd;
		DESEncryption encryption_DES;
		encryption_DES.SetKey(key,8);
		do
		{	memset(input,'\0',8);
			NotEnd = fread(&input,8,1,fin);
			encryption_DES.ProcessBlock( input, output );
			fwrite(output,8,1,fout);
		}while(NotEnd);
	}
	else //����
	{	DESDecryption decryption_DES;    
		decryption_DES.SetKey(key,8);
		while(fread(&input,8,1,fin))
		{	decryption_DES.ProcessBlock(input,output);
			fwrite(output,8,1,fout);
		}
	}
}

string Hash(char path[],char hash[])  //�����Լ��
{	CryptoPP::MD5 md;
	const size_t size = CryptoPP::MD5::DIGESTSIZE * 2;
	byte buf[size] = {0};
	string strPath = string(path);
	CryptoPP::FileSource(strPath.c_str(), true, new CryptoPP::HashFilter(md, new CryptoPP::HexEncoder( new CryptoPP::ArraySink(buf, size))));
	string strHash = string(reinterpret_cast<const char*>(buf), size);
	strcpy(hash, strHash.c_str());
	return strHash;
}
//����ǩ��
void mySign(char path[],FILE *fin,FILE *fout,bool type) 
{	char hash[17]={'\0'},output[512]={'\0'},contain[1]={'\0'}; 
	string strhash = Hash(path,hash);
	RandomPool randPool;
    char priKey[] = "pri";
	char pubKey[] = "pub";
	char seed[] = "seed";
	
	if(type)    //���ͷ�
	{	GenerateRSAKey(1024, priKey, pubKey, seed);
		cout<<"ԭժҪ:   "<<strhash<<endl;
		string xxx = RSAEncryptString(pubKey, seed, strhash);  // RSA ����
		cout << "����ժҪ: " << xxx << endl <<endl;
		strcpy(output, xxx.c_str());
		fwrite(output,xxx.length(),1,fout);
		while(fread(contain,1,1,fin)) fwrite(contain,1,1,fout);
	}
	else       //���շ�
	{	char xxx[512]={'\0'};
		fread(xxx,256,1,fin);
		while(fread(contain,1,1,fin)) fwrite(contain,1,1,fout);
		fclose(fout);
		string strhash = Hash(path,hash);
		string newhash = RSADecryptString(priKey, string(xxx));  // RSA ����
		cout<<"ժҪ�Ա�:\t"<<newhash<<"\n\t\t"<<strhash<< endl;
		fout=fopen(path,"rb");
	}
}
//rsa��ȡ��Կ
void GenerateRSAKey(unsigned int keyLength, const char *privFilename, const char *pubFilename, const char *seed)
{	RandomPool randPool;
	randPool.IncorporateEntropy((byte *)seed, strlen(seed));
	RSAES_OAEP_SHA_Decryptor priv(randPool, keyLength);
	HexEncoder privFile(new FileSink(privFilename));
	priv.AccessMaterial().Save(privFile);
	privFile.MessageEnd();
	RSAES_OAEP_SHA_Encryptor pub(priv);
	HexEncoder pubFile(new FileSink(pubFilename));
	pub.AccessMaterial().Save(pubFile);
	pubFile.MessageEnd();
}
//rsa����
string RSAEncryptString(const char *pubFilename, const char *seed, const string message)
{	FileSource pubFile(pubFilename, true, new HexDecoder);
	RSAES_OAEP_SHA_Encryptor pub(pubFile);
	RandomPool randPool;
	randPool.IncorporateEntropy((byte *)seed, strlen(seed));
	std::string result;
	StringSource(message, true, new PK_EncryptorFilter(randPool, pub, new HexEncoder(new StringSink(result))));
	return result;
}
//rsa����
string RSADecryptString(const char *privFilename, const string ciphertext)
{	FileSource privFile(privFilename, true, new HexDecoder);
	RSAES_OAEP_SHA_Decryptor priv(privFile);
	std::string result;
	StringSource(ciphertext, true, new HexDecoder(new PK_DecryptorFilter(GlobalRNG(), priv, new StringSink(result))));
	return result;
}

void putkey(FILE *fin,FILE *fout,bool type)
{	RandomPool randPool;
	char priKey[] = "pri";
	char pubKey[] = "pub";
	char seed[] = "seed";
	char output[512]={'\0'};
	if(type)
	{	FILE *fpr=fopen("key.txt","wb");
		unsigned char key[8]={89,146,116,197,155,228,172,93};
		string str = (char*)key;
		string xxx = RSAEncryptString(pubKey, seed, str);
		strcpy(output, xxx.c_str());
		fwrite(output,xxx.length(),1,fpr);
	}
	else
	{	FILE *fpr=fopen("key.txt","rb");
		fread(output,256,1,fpr);
		string str = RSADecryptString(priKey, string(output));
		strcpy(output, str.c_str());
		fclose(fpr);
		char key[128]={'\0'};
		strcpy(key, str.c_str());
		fpr=fopen("key.txt","wb");
		fwrite(key,str.length(),1,fpr);
	}
}
