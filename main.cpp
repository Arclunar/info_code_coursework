/***************************************************
Channel Coding Course Work: conolutional codes
This program template has given the message generator, BPSK modulation, AWGN channel model and BPSK demodulation,
you should first determine the encoder structure, then define the message and codeword length, generate the state table, write the convolutional encoder and decoder.
 
If you have any question, please contact me via e-mail: wuchy28@mail2.sysu.edu.cn
***************************************************/


// 包含各种库
#define  _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<math.h>
#include<string>
#include<string.h>
#include<iostream>
#include<limits.h>

using namespace std;




#define message_length 8 //the length of message
#define codeword_length 16 //the length of codeword
float code_rate = (float)message_length / (float)codeword_length;

// channel coefficient
#define pi 3.1415926
double N0, sgm;


int state_table[8][4];//state table, the size should be defined yourself
int state_num;//the number of the state of encoder structure

int message[]={0,1,1,0,0,1,0,0};
int codeword[codeword_length];//message and codeword
int re_codeword[codeword_length];//the received codeword
int de_message[message_length];//the decoding message

double tx_symbol[codeword_length][2];//the transmitted symbols
double rx_symbol[codeword_length][2];//the received symbols

void statetable();
void encoder();
void modulation();
void demodulation();
void channel();
void decoder();

// 自定义函数
int compare(int ID/*比较的ID*/,int rx/*被比较的符号*/);

int  main()
{
	int i;
	float SNR, start, finish; // 一开始程序读入
	long int bit_error, seq, seq_num;
	double BER;
	double progress;

	//generate state table //生成状态表？？？
	//statetable();

	//random seed
	srand((int)time(0));

	//input the SNR and frame number
	// printf("\nEnter start SNR: ");
	// scanf("%f", &start);
	// printf("\nEnter finish SNR: ");
	// scanf("%f", &finish);
	// printf("\nPlease input the number of message: ");
	// scanf("%d", &seq_num);

#define TEST
#ifdef TEST
#endif

#define START_EMUM
#ifdef START_EMUM
	for (SNR = start; SNR <= finish; SNR++)
	{
		//channel noise
		N0 = (1.0 / code_rate) / pow(10.0, (float)(SNR) / 10.0);
		sgm = sqrt(N0 / 2);
		
		// 误比特率
		bit_error = 0;


		// 进行 seq_num次蒙特卡洛仿真
		for (seq = 1; seq<=seq_num; seq++) 
		{
			//generate binary message randomly
			/****************
			Pay attention that message is appended by 0 whose number is equal to the state of encoder structure.
			****************/

			// 有补零操作
			for (i = 0; i<message_length - state_num; i++)
			{
				message[i] = rand() % 2;
			}
			for (i = message_length - state_num; i<message_length; i++)
			{
				message[i] = 0;
			}

			//卷积码编码，我们自己写
			//convolutional encoder
			encoder();

			//BPSK modulation
			modulation();

			//AWGN channel
			channel();

			//BPSK demodulation, it's needed in hard-decision Viterbi decoder
			//demodulation();

			//卷积码译码，我们自己写
			//convolutional decoder
			decoder();

			//calculate the number of bit error
			for (i = 0; i<message_length; i++)
			{
				if (message[i] != de_message[i])
					bit_error++;
			}

			
			//进度 百分比 
			progress = (double)(seq * 100) / (double)seq_num;

			//calculate the BER
			//计算误码率
			BER = (double)bit_error / (double)(message_length*seq);

			//print the intermediate result
			printf("Progress=%2.1f, SNR=%2.1f, Bit Errors=%2.1d, BER=%E\r", progress, SNR, bit_error, BER);
		}

		//calculate the BER
		BER = (double)bit_error / (double)(message_length*seq_num);

		//print the final result
		printf("Progress=%2.1f, SNR=%2.1f, Bit Errors=%2.1d, BER=%E\n", progress, SNR, bit_error, BER);
	}
#endif

	system("pause");
	return 0;
}

//function ： statetable
//description ： 初始化statetable[][]
//input : void 
//output : void 
void statetable()
{
	//映射关系
	//0：00/A， 1：01/B 2：10/C 3：11/D
	//第一列 输入 
	//第二列 起始点 
	//第三列 终点 
	//第四列 输出值 
	//
	int table[8][4]={
	{0,0b00,0b00,0b00},
	{1,0b00,0b10,0b11},
	{0,0b01,0b00,0b11},
	{1,0b01,0b10,0b00},
	{0,0b10,0b01,0b10},
	{1,0b10,0b11,0b01},
	{0,0b11,0b01,0b01},
	{1,0b11,0b11,0b10}
	};
	for(int i=0;i<8;i++)
	{
		for(int j=0;j<4;j++)
			state_table[i][j]=table[i][j];
	}
}		

//function ： encoder
//description ： 卷积码编码
//input : void 但利用全局变量 message数组 长度是 message_length
//output : void 但将结果写出到 tx_symbol
void encoder()
{	
	int current_output[2];

	//记录目前状态
	enum state {A,B,C,D};  // A:00 B:10 C:01 D:11
	state current_state=A;
	for(int i=0;i<message_length;i++)
	{
		switch (current_state)
		{
		case A:
			if(message[i]==0) {current_state=A; current_output[0]=0;current_output[1]=0;}
			else {current_state=B; current_output[0]=1;current_output[1]=1;}
			break;
		case B:
			if(message[i]==0) {current_state=C; current_output[0]=1;current_output[1]=0;}
			else {current_state=D; current_output[0]=0;current_output[1]=1;}
			break;
		case C:
			if(message[i]==0) {current_state=A; current_output[0]=1;current_output[1]=1;}
			else {current_state=B; current_output[0]=0;current_output[1]=0;}
			break;
		case D:
			if(message[i]==0) {current_state=C; current_output[0]=0;current_output[1]=1;}
			else {current_state=D; current_output[0]=1;current_output[1]=0;}
			break;
		default:
			break;
		}
		cout<<current_state<<endl;
		codeword[2*i]=current_output[0];
		codeword[2*i+1]=current_output[1];
	}
	

}


//调制函数
// input : void ， 但利用全局变量 codeword
// output : void ， 但写入全局变量 tx_symbol
void modulation()
{
	//BPSK modulation
	int i;

	//0 is mapped to (1,0) and 1 is mapped to (-1,0)
	for (i = 0; i<codeword_length; i++)
	{
		tx_symbol[i][0] = -1 * (2 * codeword[i] - 1); // 0 对 (1.0) ， 1 对 (-1,0)
		tx_symbol[i][1]=0; // x+0j
	}
}

//function ： channel
//description ： 发送信号加上随机高斯白噪声得到接收信号
//input : void 但利用全局变量 tx_symbol
//output : void 但将结果写入全局变量 rx_symbol
void channel()
{
	//AWGN channel
	int i, j;
	double u, r, g;

	for (i = 0; i<codeword_length; i++)
	{
		for (j = 0; j<2; j++)
		{
			u=(float)rand()/(float)RAND_MAX;  //得到0到1之间的数
			if(u==1.0)
				u=0.999999;
			r=sgm*sqrt(2.0*log(1.0/(1.0-u))); //加性高斯白噪声的幅度

			u=(float)rand()/(float)RAND_MAX; //用另一个随机数
			if(u==1.0)
				u=0.999999;
			g=(float)r*cos(2*pi*u);  //加性高斯白噪声的相位

			rx_symbol[i][j]=tx_symbol[i][j]+g;
		}
	}
}


//function ： demodulation
//description ： 解调，对接收信号进行判决
//input : void 但利用全局变量 rx_symbol
//output : void 但将结果写入全局变量 re_codeword
void demodulation()
{
	int i;
	double d1, d2;
	for (i = 0; i<codeword_length; i++)
	{
		// 计算欧式距离，比较，判断
		d1 = (rx_symbol[i][0] - 1)*(rx_symbol[i][0] - 1) + rx_symbol[i][1] * rx_symbol[i][1];
		d2 = (rx_symbol[i][0] + 1)*(rx_symbol[i][0] + 1) + rx_symbol[i][1] * rx_symbol[i][1];
		if (d1<d2)
			re_codeword[i] = 0;
		else
			re_codeword[i] = 1;	//convolution encoder, the input is message[] and the output is codeword[]

	}
}

//function ： channel
//description ： 发送信号加上随机高斯白噪声得到接收信号
//input : void 但利用全局变量 tx_symbol
//output : void 但将结果写入全局变量 rx_symbol
void decoder()
{ 
	//用到两个矩阵
	//一个累计距离，另一个记录ID
	//不能取的边（无穷大）用2*message_length+1代替，因为绝对不能可能错的比本身的长度还大
	//难题是，怎么判断该边是否存在
	//解决方法：全部节点的累计初始值都射程郑无穷，并且计算到最后的时候直接铲到最后以列，从00的位置开始回溯即可
	//但是仍然第一列还是要特殊处理以下
	int inf=2*message_length+1;
	int path_metrics_table[4][message_length+1]; //累计距离矩阵
	int trellis_trans_ID_table[4][message_length]; 	//ID记录矩阵
	//初始化累计错误全部设成无穷
	for(int i=0;i<4;i++){for(int j=0;j<message_length+1;j++) path_metrics_table[i][j]=inf;}
	//初始化刚开始的00节点的累计是0
	path_metrics_table[0][0]=0;



	//decode！
	//按接收符号两个两个来比较
	for(int i=0;i<message_length;i++)
	{
		int rx_bit=re_codeword[2*i+1]<<1+re_codeword[2*i];//组装成一个数
		for(int id=0;id<8;id++) 	//8条边按顺序比较，把结果放在下一个节点上，值小的放上去
		{
			int next_node=state_table[id][2];
			int this_path_output=state_table[id][3];
			int dist=compare(this_path_output,rx_bit);

			if(dist<path_metrics_table[next_node][i+1]) //当前的距离小于终点节点上已有的距离
			{
				path_metrics_table[next_node][i+1]=dist;
				trellis_trans_ID_table[next_node][i]=id;
			}
		}
	}

	int current_trans_ID;
	int current_node=0x00;
	//从最后一个00开始回溯
	for(int i=message_length-1;i>0;i--)
	{
		current_trans_ID=trellis_trans_ID_table[current_node][i];
		de_message[i]=state_table[current_trans_ID][0]; //当前ID对应的输入
		current_node=state_table[current_trans_ID][1]; //当前ID的起始点	
	}



	//累计错误的矩阵
	//每个节点有固定的两个来源边，计算这些边的前面节点以及该边与接收符号的差值之和，取小值放在下一个节点
	//ID对应关系
	//边ID 终点节点
	//1，3 00 A
	//5，7 01 C
	//2,4 10 B
	//6,8 11 D


}

int compare(int ID/*比较的ID*/,int rx/*被比较的符号*/)
{
	switch (state_table[ID][3]^rx)
	{
		case 0b00 :return 0;
		case 0b01:return 1;
		case 0b10:return 1;
		case 0b11:return 2;
	}


}