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

#define message_length 1000 //the length of message
#define codeword_length 2000 //the length of codeword
float code_rate = (float)message_length / (float)codeword_length;

// channel coefficient
#define pi 3.1415926
double N0, sgm;

int state_table[8][4];//state table, the size should be defined yourself
int state_num;//the number of the state of encoder structure

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

void main()
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
	printf("\nEnter start SNR: ");
	scanf("%f", &start);
	printf("\nEnter finish SNR: ");
	scanf("%f", &finish);
	printf("\nPlease input the number of message: ");
	scanf("%d", &seq_num);

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
		}

		//calculate the BER
		BER = (double)bit_error / (double)(message_length*seq_num);

		//print the final result
		printf("Progress=%2.1f, SNR=%2.1f, Bit Errors=%2.1d, BER=%E\n", progress, SNR, bit_error, BER);
	}
	system("pause");
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
        // c_t'1 t'时刻第一个符号
        // yt-s2
        double yt_s0 = (rx_symbol[2*i+j][0] - 1)*(rx_symbol[2*i+j][0] - 1) + rx_symbol[2*i+j][1] * rx_symbol[2*i][1];
         // Pch(c=0)
         // c_t'2 t'时刻第二个符号
        Pch[i][j][0]=1/sqrt(pi*N0)*exp((-1)*yt_s0/N0);
        // Pch(c=1)
        double yt_s1 = (rx_symbol[2*i+j][0] + 1)*(rx_symbol[2*i+j][0] + 1) + rx_symbol[2*i+j][1] * rx_symbol[2*i][1];
        Pch[i][j][1]=1/sqrt(pi*N0)*exp((-1)*yt_s1/N0);
        }
    }

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
            first=state_table[j][3]>>1; // 输出的第一个符号
            second=state_table[j][3]%2; // 输出的第二个符号
            state_trans_prob[i][j]=Pa*Pch[i][0][first]*Pch[i][1][second];
        }
    }

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
            node_available[i][j]=true;
        }
    }
    begin_prob[0b00][0]=1; // 第一个00点概率为1
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
    
    
    // 
}