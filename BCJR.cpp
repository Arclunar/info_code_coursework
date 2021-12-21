/***************************************************
Channel Coding Course Work: conolutional codes
This program template has given the message generator, BPSK modulation, AWGN channel model and BPSK demodulation,
you should first determine the encoder structure, then define the message and codeword length, generate the state table, write the convolutional encoder and decoder.
 
If you have any question, please contact me via e-mail: wuchy28@mail2.sysu.edu.cn
***************************************************/

#define  _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<math.h>
#include<iostream>
#include<iomanip>
#define message_length 1000 //the length of message
#define codeword_length 2000 //the length of 

#define tran_input 0
#define tran_begin 1
#define tran_end 2
#define tran_output 3
//调试编译标志

#define SIMULATION

// #define DEBUG
#ifdef DEBUG
#define TEST
#define OUTPUTGAMMA
#define OUTPUTA
#define OUTPUTB
#endif
float code_rate = (float)message_length / (float)codeword_length;

// channel coefficient
#define pi 3.1415926
using namespace std;

double N0, sgm;

int state_table[8][4];//state table, the size should be defined yourself
int state_num=2;//the number of the state of encoder structure

int message[message_length], codeword[codeword_length];//message and codeword
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

int main(int argc,char** argv)
{
	int i;
	float SNR, start, finish;
	long int bit_error, seq, seq_num;
	double BER;
	double progress;



	//generate state table
	statetable();

	//random seed
	srand((int)time(0));

	//input the SNR and frame number
	// printf("\nEnter start SNR: ");
	// scanf("%f", &start);
	// printf("\nEnter finish SNR: ");
	// scanf("%f", &finish);
	// printf("\nPlease input the number of message: ");
	// scanf("%d", &seq_num);

//#define TEST
#ifdef TEST
//编码
	for (i = 0; i<message_length - state_num; i++)
	{
		message[i] = rand() % 2;
	}
	for (i = message_length - state_num; i<message_length; i++)
	{
		message[i] = 0;
	}

	cout<<"message:";
	for(int i=0;i<message_length;i++) cout<<message[i];
	cout<<endl;

	encoder();

	cout<<"codeword:";
	for(int i=0;i<codeword_length;i++)
	{
		cout<<codeword[i];
	}
	cout<<endl;
	//调制
	modulation();

	//信道
	SNR=10;
	//channel noise
	N0 = (1.0 / code_rate) / pow(10.0, (float)(SNR) / 10.0);
	sgm = sqrt(N0 / 2);
	channel();
	// for(int i=0;i<codeword_length;i++)
	// {
	// 	for(int j=0;j<2;j++)
	// 		rx_symbol[i][j]=tx_symbol[i][j];
	// }

	cout<<endl;
	//解调
	//demodulation();
	// cout<<"re_codeword:";
	// for(int i=0;i<codeword_length;i++)
	// {
	// 	cout<<re_codeword[i];
	// }
	//译码
	decoder();
	cout<<"de_message:";
	for(int i=0;i<message_length;i++)
	{
		cout<<de_message[i];
	}
	cout<<endl;


#endif

// #define SIMULATION
#ifdef SIMULATION
	start=atof(argv[1]);
    finish=atof(argv[2]);
    seq_num=atol(argv[3]);
	for (SNR = start; SNR <= finish; SNR++)
	{
		//channel noise
		N0 = (1.0 / code_rate) / pow(10.0, (float)(SNR) / 10.0);
		sgm = sqrt(N0 / 2);
		
		bit_error = 0;

		for (seq = 1; seq<=seq_num; seq++)
		{
			//generate binary message randomly
			/****************
			Pay attention that message is appended by 0 whose number is equal to the state of encoder structure.
			****************/
			for (i = 0; i<message_length - state_num; i++)
			{
				message[i] = rand() % 2;
			}
			for (i = message_length - state_num; i<message_length; i++)
			{
				message[i] = 0;
			}

			//convolutional encoder
			encoder();

			//BPSK modulation
			modulation();

			//AWGN channel
			channel();

			//BPSK demodulation, it's needed in hard-decision Viterbi decoder
			//demodulation();

			//convolutional decoder
			decoder();

			//calculate the number of bit error
			for (i = 0; i<message_length; i++)
			{
				if (message[i] != de_message[i])
					bit_error++;
			}

			progress = (double)(seq * 100) / (double)seq_num;

			//calculate the BER
			BER = (double)bit_error / (double)(message_length*seq);

			//print the intermediate result
			printf("Progress=%2.1f, SNR=%2.1f, Bit Errors=%2.1d, BER=%E\r", progress, SNR, bit_error, BER);
			if(bit_error>200)
			{
				BER = (double)bit_error / (double)(message_length*seq);
				break;
			}
		}

		//calculate the BER
		//BER = (double)bit_error / (double)(message_length*seq_num);

		//print the final result
		printf("Progress=%2.1f, SNR=%2.1f, Bit Errors=%2.1d, BER=%E\n", progress, SNR, bit_error, BER);
	}
#endif
	//system("pause");
	return 0;
}
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

void encoder()
{
	//convolution encoder, the input is message[] and the output is codeword[]
	int current_output[2];

	//记录目前状态
	//enum state {A,B,C,D};  // A:00 B:10 C:01 D:11
	int current_state=0b00;
	for(int i=0;i<message_length;i++)
	{
		switch (current_state)
		{
		case 0b00:
			if(message[i]==0) {current_state=0b00; current_output[0]=0;current_output[1]=0;}
			else {current_state=0b10; current_output[0]=1;current_output[1]=1;}
			break;
		case 0b01:
			if(message[i]==0) {current_state=0b00; current_output[0]=1;current_output[1]=1;}
			else {current_state=0b10; current_output[0]=0;current_output[1]=0;}
			break;
		case 0b10:
			if(message[i]==0) {current_state=0b01; current_output[0]=1;current_output[1]=0;}
			else {current_state=0b11; current_output[0]=0;current_output[1]=1;}
			break;
		case 0b11:
			if(message[i]==0) {current_state=0b01; current_output[0]=0;current_output[1]=1;}
			else {current_state=0b11; current_output[0]=1;current_output[1]=0;}
			break;
		default:
			break;
		}
		//cout<<current_state<<endl;
		codeword[2*i]=current_output[0];
		codeword[2*i+1]=current_output[1];
	}
	
}

void modulation()
{
	//BPSK modulation
	int i;

	//0 is mapped to (1,0) and 1 is mapped tp (-1,0)
	for (i = 0; i<codeword_length; i++)
	{
		tx_symbol[i][0] = -1 * (2 * codeword[i] - 1);
		tx_symbol[i][1]=0;
	}
}
void channel()
{
	//AWGN channel
	int i, j;
	double u, r, g;

	for (i = 0; i<codeword_length; i++)
	{
		for (j = 0; j<2; j++)
		{
			u=(float)rand()/(float)RAND_MAX;
			if(u==1.0)
				u=0.999999;
			r=sgm*sqrt(2.0*log(1.0/(1.0-u)));

			u=(float)rand()/(float)RAND_MAX;
			if(u==1.0)
				u=0.999999;
			g=(float)r*cos(2*pi*u);

			rx_symbol[i][j]=tx_symbol[i][j]+g;
		}
	}
}
void demodulation()
{
	int i;
	double d1, d2;
	for (i = 0; i<codeword_length; i++)
	{
		d1 = (rx_symbol[i][0] - 1)*(rx_symbol[i][0] - 1) + rx_symbol[i][1] * rx_symbol[i][1];
		d2 = (rx_symbol[i][0] + 1)*(rx_symbol[i][0] + 1) + rx_symbol[i][1] * rx_symbol[i][1];
		if (d1<d2)
			re_codeword[i] = 0;
		else
			re_codeword[i] = 1;
	}
}



// input : re_symbol
// output : re_message
void decoder()
{ 
	// BCJR
    // 利用re_symbol 计算 信道转移概率
    // 先验概率 Pa 置为1
    // 码率 0.5
    // 计算 信道观察
    
    // 三维数组，每个接收符号有一个0的信道观察和1的信道观察
    // 第1维 ： 第t'时刻
    // 第2维 ： 该时刻的第1/2个码元符号
    // 第3维 ： 关于0/1的信道观察
    double Pch[message_length][2][2];
    for(int i=0;i<message_length;i++)
    {   
        for(int j=0;j<2;j++)
        {
		//t'时刻第j+1个符号
        // c_t'1 

        // yt-s0 和0对应的符号的距离平方
        double yt_s0 = (rx_symbol[2*i+j][0] - 1)*(rx_symbol[2*i+j][0] - 1) + rx_symbol[2*i+j][1] * rx_symbol[2*i+j][1];
         // Pch(c=0)
		//  cout<<"rx_symbol[2*i+j][0]="<<rx_symbol[2*i+j][0]<<endl;
		//  cout<<"rx_symbol[2*i+j][1]="<<rx_symbol[2*i+j][1]<<endl;
		//  cout<<"yt_s0 "<<yt_s0<<endl;
        
        Pch[i][j][0]=1/sqrt(pi*N0)*exp((-1)*yt_s0/N0);

		// yt-s1 和1对应的符号的距离平方
        double yt_s1 = (rx_symbol[2*i+j][0] + 1)*(rx_symbol[2*i+j][0] + 1) + rx_symbol[2*i+j][1] * rx_symbol[2*i+j][1];
        
		// Pch(c=1)
		Pch[i][j][1]=1/sqrt(pi*N0)*exp((-1)*yt_s1/N0);
        }
    }
	// cout<<endl;
	// cout<<"Pch"<<endl;
	// for(int i=0;i<message_length;i++)
	// {
	// 	for(int j=0;j<2;j++)
	// 	{
	// 		for(int k=0;k<2;k++)
	// 			cout<<fixed<<setprecision(2)<<Pch[i][j][k]<<"  ";
	// 		cout<<endl;
	// 	}
	// }
	// cout<<endl;


    // 状态转移概率 gamma
    // 第1维 时刻
    // 第2维 8条边
    double state_trans_prob[message_length][8];
    // 计算 0.5 * 边的第一个符号输出对应的信道观察 * 边的第二个符号输出对应的信道观察
    for(int i=0;i<message_length;i++)
    {

        int first;
        int second;
        double Pa=0.5;
        for(int j=0;j<8;j++)
        {
            first=state_table[j][tran_output]>>1; // 输出的第一个符号
            second=state_table[j][tran_output]%2; // 输出的第二个符号
            state_trans_prob[i][j]=Pa*Pch[i][0][first]*Pch[i][1][second];
			
        }
    }


#ifdef OUTPUTGAMMA

	//输出信道观察看看
	cout<<endl;
	cout<<"Gamma:"<<endl;
	for(int i=0;i<8;i++)
	{
		for(int j=0;j<message_length;j++)
			cout<<setprecision(3)<<state_trans_prob[j][i]<<" ";
		cout<<endl;
	}
	cout<<endl;
#endif

    // Probability of beginning a trellis trasition
    double begin_prob[4][message_length+1];
    // Probability of ending a trellis trasition
    double end_prob[4][message_length+1];
    // 结点是否合法
    bool node_available[4][message_length+1];
    // 初始化
    for(int i=0;i<4;i++)
    {
        for(int j=0;j<message_length+1;j++)
        {
            
            begin_prob[i][j]=0;
			end_prob[i][j]=0;
            node_available[i][j]=true;
        }
    }
    begin_prob[0b00][0]=1; // 第一个00点概率为1
	end_prob[0b00][message_length]=1; //最后一个00点的作为结尾的概率是1
    //开头
    //第一列
    node_available[0b01][0]=false;
    node_available[0b10][0]=false;
    node_available[0b11][0]=false;
    //第二列
    node_available[0b01][1]=false;
    node_available[0b11][1]=false;
    //结尾
    //倒数第二列
    node_available[0b10][message_length-1]=false;
    node_available[0b11][message_length-1]=false;
    //最后一列
    node_available[0b01][message_length]=false;
    node_available[0b10][message_length]=false;
    node_available[0b11][message_length]=false;
    
    

    // 开始计算A
	for(int i=1;i<message_length+1;i++)
	{
		for(int node=0;node<4;node++)
		{
			double sum=0;
			for(int tran_ID=0;tran_ID<8;tran_ID++)
			{
				// 如果边的输出为当前结点，且边的起点是合法的
				int begin_node=state_table[tran_ID][tran_begin];
				int end_node=state_table[tran_ID][tran_end];
				if(end_node==node && node_available[begin_node][i-1]==true && node_available[end_node][i]==true)
					{
						//加上起点结点作为起点的概率 * 这条边的信道观察
						sum+=begin_prob[begin_node][i-1]*state_trans_prob[i-1][tran_ID];
					}
			}
			begin_prob[node][i]=sum;
		}
		//归一化
		double sum_prob =0;
		for(int node=0;node<4;node++)
		{
			sum_prob+=begin_prob[node][i]; //计算分母
		}
		for(int node=0;node<4;node++)
		{
			begin_prob[node][i]=begin_prob[node][i]/sum_prob;
		}
	}

#ifdef OUTPUTA
	//输出A看看
	cout<<endl;
	cout<<"A:"<<endl;
	for(int i=0;i<4;i++)
	{
		for(int j=0;j<message_length+1;j++)
			cout<<fixed<<begin_prob[i][j]<<"  ";
		cout<<endl;
	}
	cout<<endl;
#endif

	//计算B
	for(int i=message_length-1;i>=0;i--) //从倒数第二列开始 最后一列的索引是message_length
	{
		for(int node=0;node<4;node++)
		{
			double sum=0;
			for(int tran_ID=0;tran_ID<8;tran_ID++)
			{
				int begin_node = state_table[tran_ID][tran_begin];
				int end_node=state_table[tran_ID][tran_end];
				if(begin_node==node && node_available[end_node][i+1]==true && node_available[begin_node][i]==true)
				{
					sum+=end_prob[end_node][i+1]*state_trans_prob[i][tran_ID];
				}
			}
			end_prob[node][i]=sum;
		}
		//归一化
		double sum_prob=0;
		for(int node=0;node<4;node++)
			sum_prob+=end_prob[node][i];
		for(int node=0;node<4;node++)
			end_prob[node][i]=end_prob[node][i]/sum_prob;
	}

#ifdef OUTPUTB
	//输出B看看
	cout<<endl;
	cout<<"B:"<<endl;
	for(int i=0;i<4;i++)
	{
		for(int j=0;j<message_length+1;j++)
			cout<<fixed<<end_prob[i][j]<<"  ";
		cout<<endl;
	}
	cout<<endl;
#endif

	//开始判断
	for(int i=0;i<message_length;i++)
	{
		//索引为0 2 4 6是输入0
		//索引为1 3 5 7是输入1
		//罗马尼亚国旗
		//计算0的概率
		double P_is0=0;
		double P_is1=0;
		int begin_node;
		int end_node;
		for(int tran_ID=0;tran_ID<8;tran_ID+=2)
		{
			begin_node=state_table[tran_ID][tran_begin];
			end_node=state_table[tran_ID][tran_end];
			P_is0+=begin_prob[begin_node][i]*state_trans_prob[i][tran_ID]*end_prob[end_node][i+1];
		}
		for(int tran_ID=1;tran_ID<8;tran_ID+=2)
		{
			begin_node=state_table[tran_ID][tran_begin];
			end_node=state_table[tran_ID][tran_end];
			P_is1+=begin_prob[begin_node][i]*state_trans_prob[i][tran_ID]*end_prob[end_node][i+1];
		}
		if(P_is0>P_is1) de_message[i]=0;
		else de_message[i]=1;
	}

}