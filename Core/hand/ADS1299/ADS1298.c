/*------------------------------- DEFINES-----------------------------*/

#include "ADS1298.h"
#include "M8266WIFIDrv.h"
#include "M8266WIFI_ops.h"
#include "adc.h"
#include "usart.h"
#include "spo2.h"

#define Lead_off_detect 0
#define Right_leg_drive 1
#define RDATAC 0	//连续读取模式

/*------------------------------- VARIABLES -----------------------------*/
bool INIT_DRDY = false; // Flag to ready data from ADS1298
bool start_send_flag = false;
bool isDaisy = false;		// does this have a daisy chain board?

int32_t stat_1, stat_2;    	///用于保存单板1和2的状态寄存器

//uint32_t ads_save_count=0;
//uint8_t ads_save_buff[800];

uint8_t regData[24];		// array with data from all registers??17??????????

uint32_t test500hz_count=0;

int32_t channelData [8];	// array used when reading channel data board 1+2
int32_t Zero_deviation_ch1=25;//
//int32_t Zero_deviation_ch1=1000;
int32_t Zero_deviation_ch2=20;
channel_union channelvalue[4];

int lead_open_flag=0;

uint8_t ecg_eeg_sendbuff[4][8];


//**********************************notching_filter_Q=3*******************************************//
//double NUM[4] = {0.004750523610981,  0.01425157083294,  0.01425157083294, 0.004750523610981};
//double DEN[4] = {1,   -2.250085081726,    1.756401381786,  -0.4683121111717};
//**********************************lowpass_filter_30hz_5??*******************************************//
//const double NUM[6] = {//30hz???5??
//  0.0001366356754708,0.0006831783773542, 0.001366356754708, 0.001366356754708,
//  0.0006831783773542,0.0001366356754708};
//const double DEN[6] = {
//                   1,   -3.782050676233,    5.837478361871,   -4.577743759762,
//      1.819272330918,  -0.2925839151789};

/*------------------------------- FUNCTIIONS -----------------------------*/
//??????????????????????????????δ????
void ADS_Init()
{
    ///ads的交流协议
	for(int j=0;j<4;j++)
	{
		for(int i=0;i<8;i++)
		{
		 ecg_eeg_sendbuff[j][i]=0;
		}
	}
	for(int j=0;j<4;j++)
	{
		ecg_eeg_sendbuff[j][0]=0xaa;
	  ecg_eeg_sendbuff[j][1]=0x01+j;
		ecg_eeg_sendbuff[j][7]=0xbb;
	}
		
	//HAL_Delay(1000);
	//HAL_GPIO_WritePin( ADS_CS_Port , ECG_CS_Pin, GPIO_PIN_SET);
//	HAL_GPIO_WritePin(GPIOE, ADS_PWDN_Pin, GPIO_PIN_SET);
//	HAL_GPIO_WritePin(GPIOE, ADS_RESET_Pin, GPIO_PIN_SET);
	
	// INIT ADS1298 and ADS1299
	// INIT ADS1298 and ADS1299
 HAL_GPIO_WritePin(GPIOE,ECG_CS_Pin,GPIO_PIN_SET);
 HAL_GPIO_WritePin(GPIOE,EEG_CS_Pin,GPIO_PIN_SET);
 HAL_GPIO_WritePin(GPIOE, EEG_PWDN_Pin, GPIO_PIN_SET);
 HAL_GPIO_WritePin(GPIOE, EEG_RESET_Pin, GPIO_PIN_SET); 
 HAL_GPIO_WritePin(GPIOE, ECG_PWDN_Pin, GPIO_PIN_SET);
 HAL_GPIO_WritePin(GPIOE, ECG_RESET_Pin, GPIO_PIN_SET); 
 HAL_Delay(1000);
 while(regData[0]!=0x3c)
 {
   ADS_RESET(ECG_CS_Pin);
   HAL_Delay(500);
    ADS_SDATAC(ECG_CS_Pin);
     HAL_Delay(500);
    ADS_RREG(0,ECG_CS_Pin);//??17?????????????????
 }
 while(regData[0]!=0x3c)
 {
   ADS_RESET(EEG_CS_Pin);
   HAL_Delay(500);
    ADS_SDATAC(EEG_CS_Pin);
     HAL_Delay(500);
    ADS_RREG(0,EEG_CS_Pin);//??17?????????????????
 }
//	HAL_GPIO_WritePin(GPIOE, EEG_PWDN_Pin, GPIO_PIN_SET);
//	HAL_GPIO_WritePin(GPIOE, EEG_RESET_Pin, GPIO_PIN_SET);	
//	HAL_GPIO_WritePin(GPIOE, ECG_PWDN_Pin, GPIO_PIN_SET);
//	HAL_GPIO_WritePin(GPIOE, ECG_RESET_Pin, GPIO_PIN_SET);	
	
	
//	HAL_Delay(1000);
//		

//	ADS_RESET(EEG_CS_Pin);//复位
//	ADS_RESET(ECG_CS_Pin);
//	HAL_Delay(500);
	
//	
//	ADS_SDATAC(EEG_CS_Pin);//停止连续读取数据模式
//	ADS_SDATAC(ECG_CS_Pin);
	HAL_Delay(500);
		
	//Work settings
	//ADS_WREG(CONFIG1,0x06);//1298???????
    ADS_WREG(CONFIG1,0xD0|CONFIG1_DR_500SPS, EEG_CS_Pin);//500sps 10010 101
	ADS_WREG(CONFIG1,0x86, ECG_CS_Pin);//500sps 10000 110
	HAL_Delay(100);
	
	//ADS_WREG(CONFIG2,0x10);//0001 0000 ;1298 
	ADS_WREG(CONFIG2,0xD0|CONFIG2_TESTSIGNAL_NOUSE, EEG_CS_Pin);//11010010
	ADS_WREG(CONFIG2,0x12|CONFIG2_TESTSIGNAL_NOUSE, ECG_CS_Pin);//00010010
	HAL_Delay(100);
	
	//ADS_WREG(CONFIG3,0xDC);  //1298
	ADS_WREG(CONFIG3,0xEC, EEG_CS_Pin);//1110 1100 ec 
	ADS_WREG(CONFIG3,0xCC, ECG_CS_Pin);//1100 1100 cc 
	HAL_Delay(100);
	
	//ADS_WREG(CH1SET,0x50); //
	ADS_WREG(CH1SET,0x40, EEG_CS_Pin); 	//EEG通道1增益，EEG只有通道1    Gain:1,2,4,6,8,12,24[6:4]:x00,x10,x20,x30,x40,x50,x60
	ADS_WREG(CH1SET,0x50, ECG_CS_Pin); 	//ECG通道1增益

//	ADS_WREG(CH1SET,0x01, EEG_CS_Pin); 	//EEG通道1增益，EEG只有通道1    Gain:1,2,4,6,8,12,24[6:4]:x00,x10,x20,x30,x40,x50,x60
//	ADS_WREG(CH1SET,0x01, ECG_CS_Pin); 	//ECG通道1增益
	HAL_Delay(10);
	
//	ADS_WREG(CH2SET,0x50, ECG_CS_Pin);  //ECG通道2增益 
//	HAL_Delay(10);
//	ADS_WREG(CH3SET,0x50, ECG_CS_Pin);  //ECG通道3增益
//	HAL_Delay(10);
	ADS_WREG(CH2SET,0xF0, ECG_CS_Pin);  //ECG通道2增益 内部短接
	HAL_Delay(10);
	ADS_WREG(CH3SET,0xF0, ECG_CS_Pin);  //ECG通道3增益
	HAL_Delay(10);
	ADS_WREG(CH4SET,0xF0, ECG_CS_Pin);  //01010000 关闭通道4
	HAL_Delay(10);
	
//	ADS_WREG(CH5SET,0xF8);  
//	HAL_Delay(10);
//	ADS_WREG(CH6SET,0xF8);  
//	HAL_Delay(10);
//	ADS_WREG(CH7SET,0xF8);  
//	HAL_Delay(10);
//	ADS_WREG(CH8SET,0xF8);  
//	HAL_Delay(10);

#if Right_leg_drive
	ADS_WREG(BIAS_SENSP,0x01,EEG_CS_Pin);  	//ADS1299 通道1
	ADS_WREG(BIAS_SENSP,0x07,ECG_CS_Pin);		//ADS1298	通道1、2、3
	HAL_Delay(10);
	ADS_WREG(BIAS_SENSN,0x01,EEG_CS_Pin);  //
	ADS_WREG(BIAS_SENSN,0x07,ECG_CS_Pin);
//	HAL_Delay(10);
#endif
#if Lead_off_detect
	ADS_WREG(LOFF,0x00);//?????????????????
	HAL_Delay(10);
	ADS_WREG(LOFF_SENSP,0x07);  //??????????????????????????
	HAL_Delay(10);
	ADS_WREG(LOFF_SENSN,0x07);  //???????1???????????????
	HAL_Delay(10);
	ADS_WREG(CONFIG4,0x02); //??????????????? 
	HAL_Delay(10);
#endif

//	//only read
//	//ADS_WREG(LOFF_STATP,0x00);  // ???????????缫??
//	//HAL_Delay(10);
//	//ADS_WREG(LOFF_STATN,0x00);  // ???????为?缫??
//	//HAL_Delay(10);
//	ADS_WREG(LOFF_FLIP,0x00);  // ??????
//	HAL_Delay(10);	
//	ADS_WREG(GPIO,0x00);  //???
//	HAL_Delay(10);
//	ADS_WREG(MISC1,0x20);  
//	HAL_Delay(10);
//	ADS_WREG(CONFIG4,0x02); //??????????????? 
//	HAL_Delay(10);
//	ADS_WREG(MISC2,0xF0);  
//	HAL_Delay(10);
//	ADS_WREG(0x18,0x0A); /wct1
//	HAL_Delay(10);
//	ADS_WREG(0x19,0xE3);  //wct2
//	HAL_Delay(10);
	
		ADS_RREGS(0,8);//??17?????????????ж???
//	for(int i=0;i<20;i++)
//	{
//		printf("regist[%d]:%x\r\n",i,regData[i]);
//	}

	HAL_Delay(1000);
#if RDATAC
	ADS_RDATAC(ECG_CS_Pin);            // enter Read Data Continuous mode
	ADS_RDATAC(EEG_CS_Pin);
	HAL_Delay(100);
#else
	ADS_START(ECG_CS_Pin); //??????????
	ADS_START(EEG_CS_Pin);
	HAL_Delay(100);
#endif
}
	
//double Notching50Hz(double y[], double x[],double xn){
//  double yn;
//	x[0]=x[1];
//  x[1]=x[2];
//  x[2]=xn;
//  y[0]=y[1];
//	y[1]=y[2];
//  yn=0.949233124f*x[2]-1.53589145f*x[1]+0.94923312f*x[0]-(-1.535891457f)*y[1]-0.89846625f*y[0];
//	y[2]=yn;
//	return yn;
//}

//double osahs_30hz_lowpass_filter(double y[], double x[],double xn)//5??
//{
//	double yn;
//  x[0]=x[1];
//  x[1]=x[2];
//  x[2]=x[3];
//  x[3]=x[4];
//  x[4]=x[5];
//  x[5]=xn;
//  y[0]=y[1];
//  y[1]=y[2];
//  y[2]=y[3];
//  y[3]=y[4];
//  y[4]=y[5];
//  yn=NUM[0]*x[5]+NUM[1]*x[4]+NUM[2]*x[3] + NUM[3]*x[2]+NUM[4]*x[2]+NUM[5]*x[0]-DEN[1]*y[4]-DEN[2]*y[3]-DEN[3]*y[2]-DEN[4]*y[1]-DEN[5]*y[0];
//  y[5]=yn;
//	return yn;
//}

//stop continuous mode
void ADS_SDATAC(uint16_t CS_PIN){
	//HAL_GPIO_WritePin(ADS_CS_GPIO_Port, ADS_CS_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin( ADS_CS_Port , CS_PIN, GPIO_PIN_RESET);
	//HAL_Delay(10);
	transferSPI(_SDATAC);
	//HAL_Delay(10);
	//HAL_GPIO_WritePin(ADS_CS_GPIO_Port, ADS_CS_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin( ADS_CS_Port , CS_PIN, GPIO_PIN_SET);
}

//start data conversion 
void ADS_START(uint16_t CS_PIN) {			
	//HAL_GPIO_WritePin(ADS_CS_GPIO_Port, ADS_CS_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(ADS_CS_Port, CS_PIN, GPIO_PIN_RESET);
	//HAL_Delay(10);
	transferSPI(_START);
	//HAL_Delay(10);
	//HAL_GPIO_WritePin(ADS_CS_GPIO_Port, ADS_CS_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(ADS_CS_Port, CS_PIN, GPIO_PIN_SET);
}

//stop data conversion 
void ADS_STOP(uint16_t CS_PIN) {			
	//HAL_GPIO_WritePin(ADS_CS_GPIO_Port,ADS_CS_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(ADS_CS_Port,CS_PIN, GPIO_PIN_RESET);
	transferSPI(_STOP);
	//HAL_GPIO_WritePin(ADS_CS_GPIO_Port,ADS_CS_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(ADS_CS_Port,CS_PIN, GPIO_PIN_SET);
}

//Reset
void ADS_RESET(uint16_t CS_PIN){
	//HAL_GPIO_WritePin(ADS_CS_GPIO_Port, ADS_CS_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(ADS_CS_Port, CS_PIN, GPIO_PIN_RESET);
	HAL_Delay(10);
	transferSPI(_RESET);
	HAL_Delay(10);
	//HAL_GPIO_WritePin(ADS_CS_GPIO_Port, ADS_CS_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(ADS_CS_Port, CS_PIN, GPIO_PIN_SET);
}

//start continuous mode
void ADS_RDATAC(uint16_t CS_PIN) {
	//HAL_GPIO_WritePin(ADS_CS_GPIO_Port,ADS_CS_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(ADS_CS_Port, CS_PIN, GPIO_PIN_RESET);
	transferSPI(_START);
	HAL_Delay(10);
	transferSPI(_RDATAC);
	//HAL_GPIO_WritePin(ADS_CS_GPIO_Port,ADS_CS_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(ADS_CS_Port, CS_PIN, GPIO_PIN_SET);
}

//
void ADS_WAKEUP(uint16_t CS_PIN) {
  //HAL_GPIO_WritePin(ADS_CS_GPIO_Port,ADS_CS_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(ADS_CS_Port, CS_PIN, GPIO_PIN_RESET);
	transferSPI( _WAKEUP);
	//HAL_GPIO_WritePin(ADS_CS_GPIO_Port,ADS_CS_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(ADS_CS_Port, CS_PIN, GPIO_PIN_SET);
}


// Register Read/Write Commands
uint8_t ADS_getDeviceID(uint16_t CS_PIN) {			// simple hello world com check
	
	uint8_t data = ADS_RREG(0, CS_PIN);
	
	return data;
}
//?????????????
uint8_t ADS_RREG(uint8_t _address, uint16_t CS_PIN){		//  reads ONE register at _address
	uint8_t opcode1 = _address + 0x20; 		//  RREG expects 001rrrrr where rrrrr = _address
	//uint8_t num;
	HAL_GPIO_WritePin(ADS_CS_Port, CS_PIN, GPIO_PIN_RESET);			//  open SPI
	//HAL_Delay(10);
	transferSPI( opcode1); 								//  opcode1
  transferSPI( 0); 											//  opcode2
  
	regData[_address] = transferSPI( 0);		//  update mirror location with returned byte
	//num = transferSPI( 0);		//  update mirror location with returned byte	
	HAL_GPIO_WritePin(ADS_CS_Port, CS_PIN, GPIO_PIN_SET);				//  close SPI	
	//HAL_Delay(10);
	return regData[_address];			// return requested register value
	//return num;			// return requested register value
}

// Read more than one register starting at _address
void ADS_RREGS(uint8_t _address, uint8_t _numRegistersMinusOne) {
//	for(byte i = 0; i < 0x17; i++){
//		regData[i] = 0;									//  reset the regData array
//	}
	int i;
	
	uint8_t opcode1 = _address + 0x20; 				//  RREG expects 001rrrrr where rrrrr = _address
	
	HAL_GPIO_WritePin(ADS_CS_Port,EEG_CS_Pin, GPIO_PIN_RESET); 					//  open SPI
	
	transferSPI( opcode1); 										//  opcode1
	transferSPI( _numRegistersMinusOne);				//  opcode2

	for(i = 0; i <= _numRegistersMinusOne; i++){
		regData[_address + i] = transferSPI( 0x00); 	//  add register byte to mirror array
//		printf("%d:%x\r\n",i,regData[_address + i]);
	}
	
	HAL_GPIO_WritePin(ADS_CS_Port,EEG_CS_Pin, GPIO_PIN_SET); 			//  close SPI
}

//  Write ONE register at _address
void ADS_WREG(uint8_t _address, uint8_t _value, uint16_t CS_PIN) {	//  Write ONE register at _address

	uint8_t opcode1 = _address + 0x40; 				//  WREG expects 010rrrrr where rrrrr = _address
	
	HAL_GPIO_WritePin(ADS_CS_Port,CS_PIN, GPIO_PIN_RESET); 					//  open SPI
	
	transferSPI( opcode1);											//  Send WREG command & address
	transferSPI( 0x00);												//	Send number of registers to read -1
	transferSPI( _value);											//  Write the value to the register
	
	HAL_GPIO_WritePin(ADS_CS_Port,CS_PIN, GPIO_PIN_SET); 						//  close SPI
	
	regData[_address] = _value;			//  update the mirror array
	
}
//д???????????
void ADS_WREGS(uint8_t _address, uint8_t _numRegistersMinusOne){
	int i;
	
	uint8_t opcode1 = _address + 0x40;				//  WREG expects 010rrrrr where rrrrr = _address
	
	HAL_GPIO_WritePin(ADS_CS_Port,EEG_CS_Pin, GPIO_PIN_RESET); 					//  open SPI
	
	transferSPI( opcode1);											//  Send WREG command & address
	transferSPI( _numRegistersMinusOne);				//	Send number of registers to read -1	
	
	for (i=_address; i <=(_address + _numRegistersMinusOne); i++){
		transferSPI( regData[i]);								//  Write to the registers
	}	
	
	HAL_GPIO_WritePin(ADS_CS_Port,EEG_CS_Pin, GPIO_PIN_SET); 						//  close SPI
}

//read data
void ADS_RDATA() {
    /// 当DRDY变低时，在停止读连续模式中使用
    ///是EEG的缓存
	uint8_t inByte,inByte1,inByte2,inByte3;
	int nchan = 4;                      	//假设8通道。如果需要，它会在后面的块中自动更改为16。
	stat_1 = 0;							    //  清除状态寄存器，stat_1->ADS1299, stat_2->ADS1298
	stat_2 = 0;
	/// CLEAR BUFF
	for(int i = 0; i < nchan; i++){
        ///四个字节的buf
		channelData[i] = 0;		            ///1~3->ECG, 0->EEG
	}


    /********************************************EEGSPI*********************************************/

	HAL_GPIO_WritePin(ADS_CS_Port,EEG_CS_Pin,GPIO_PIN_RESET); 					//  open SPI

	transferSPI( _RDATA);                                   //Read data by command; supports multiple read back
    ///给EEG写入一个命令
	///从spi中的第一个ads读取通道数据，读取寄存器数据
	for(int i = 0; i < 3; i++){
        /// 读取3字节状态寄存器(1100+LOFF_STATP+LOFF_STATN+GPIO[7:4])
		inByte = transferSPI( 0x00);
		stat_1 = (stat_1<<8) | inByte;
        ///将上一次的数据左移后再引入此次的一共三个字节
	}

	///READ EEG SIGNAL DATA
	for(int i = 0; i < 1; i++){
		inByte1 = transferSPI( 0x00);
		inByte2 = transferSPI( 0x00);
		inByte3 = transferSPI( 0x00);
		channelData[i] = (inByte1 << 16) | (inByte2 << 8) | inByte3;
        ///组成三个字节
	}
	HAL_GPIO_WritePin(ADS_CS_Port,EEG_CS_Pin,GPIO_PIN_SET); 					//  close SPI

    /*********************************************EEGSPI*********************************************/


    /*********************************************ECGSPI*********************************************/

	HAL_GPIO_WritePin(ADS_CS_Port,ECG_CS_Pin,GPIO_PIN_RESET); 					//  open SPI

	transferSPI( _RDATA);               //Read data by command; supports multiple read back
	                                    // READ CHANNEL DATA FROM FIRST ADS IN DAISY LINE, READ REGESTER DATA
	for(int i = 0; i < 3; i++){			//  read 3 byte status register (1100+LOFF_STATP+LOFF_STATN+GPIO[7:4])
		inByte = transferSPI( 0x00);
		stat_2 = (stat_2<<8) | inByte;
	}
	//READ ECG SIGNAL DATA
	for(int i = 1; i < 4; i++){
		inByte1 = transferSPI( 0x00);
		inByte2 = transferSPI( 0x00);
		inByte3 = transferSPI( 0x00);
		channelData[i] = (inByte1 << 16) | (inByte2 << 8) | inByte3;
	}

	HAL_GPIO_WritePin(ADS_CS_Port,ECG_CS_Pin,GPIO_PIN_SET); 					//  close SPI

    /*********************************************ECGSPI*********************************************/
}

// String-Byte converters for RREG and WREG

//当24位数据为负数时，用32位储存需做相应调整
int Uint24ToUint32(int NumIn)
{
    ///将24位数据，弄成32位
	 int32_t OutPut=0;
     ///如果是负数，最高一位为负数
	 if(NumIn&0x00800000)
	 {
         NumIn=0xFFFFFF-NumIn;
         OutPut = 0-NumIn;
	 }
	 else
	 {
         OutPut = NumIn;
     }
     return OutPut;
}


void ADS_sendUARTData(void){
	for(int i=0;i<3;i++)
	{
		channelvalue[i].INT32=Uint24ToUint32(channelData[i]);
	}
	channelvalue[0].INT32	=channelvalue[0].INT32+Zero_deviation_ch1;
	channelvalue[1].INT32	=channelvalue[1].INT32+Zero_deviation_ch2;
}
	
void ADS_SendData(){
	
		ADS_sendUARTData();
}

int32_t* getChannelData(){
	
	return channelData;
}

uint8_t transferSPI(uint8_t send){
	uint8_t rx = 0x00;
	HAL_SPI_TransmitReceive(&hspi1, &send, &rx, sizeof(rx),0xFFFF);
	return rx;
}

uint32_t ADS_temp;

void ADS_SendDataBuff()
{	 	
	for(int i=0;i<4;i++)
	{
        ///channelData[0]是EEG   [1][2][3]是ECG
		channelvalue[i].INT32=Uint24ToUint32(channelData[i]);
	}
/*************************pack****************************/
///buff = [4][8] =
	for(int i=0;i<2;i++)
	{
		for(int j=0;j<4;j++)
		{
			ecg_eeg_sendbuff[i][j+2]=channelvalue[i].UINT8[j];
		}
	}
	if(lead_open_flag)
        ecg_eeg_sendbuff[2][5]=1;
	else
        ecg_eeg_sendbuff[2][5]=0;
	//校验和
	for(int i=0;i<4;i++)
	{
		ecg_eeg_sendbuff[i][6]=(uint8_t)(channelvalue[i].UINT8[1]+channelvalue[i].UINT8[2]+channelvalue[i].UINT8[3]+channelvalue[i].UINT8[0]);
	}
	
}


void ADS1299_SendState(void)
{
	uint16_t status;
	M8266WIFI_SPI_Send_BlockData((uint8_t*)ecg_eeg_sendbuff[2],8,100,0,NULL,0,&status);
}
//void change_leadoff_detect(bool state)
//{
//		#if RDATAC
//		ADS_SDATAC();
//		HAL_Delay(100);
//	#else
//		ADS_STOP();
//		HAL_Delay(100);
//	#endif

//	if(state)
//	{
//			ADS_WREG(LOFF,0x20);//?????????????????
//	   HAL_Delay(10);
//		ADS_WREG(LOFF_SENSP,0xFF);  //??????????????????????????
//		HAL_Delay(10);
//		ADS_WREG(LOFF_SENSN,0xFF);  //???????1???????????????
//		HAL_Delay(10);
//		ADS_WREG(CONFIG4,0x02); //??????????????? 
//		HAL_Delay(10);
//		stat_1=0x10000000;
//		lead_open_flag=1;
//	}
//	else
//	{
//	ADS_WREG(LOFF,0x00);//?????????????????
//	HAL_Delay(10);
//		ADS_WREG(LOFF_SENSP,0x00);  //??????????????????????????
//		HAL_Delay(10);
//		ADS_WREG(LOFF_SENSN,0x00);  //???????1???????????????
//		HAL_Delay(10);
//		ADS_WREG(CONFIG4,0x00); //??????????????? 
//		HAL_Delay(10);
//		stat_1=0;
//		lead_open_flag=0;
//	}
//	HAL_Delay(1000);
//	#if RDATAC
//		ADS_RDATAC();            // enter Read Data Continuous mode
//		HAL_Delay(100);
//	#else
//		ADS_START(); //??????????
//		HAL_Delay(100);
//	#endif
//}

//脑电采集控制，state=1: 开启通道 state=0：关闭通道
void start_ch2_sample(bool state)
{
	#if RDATAC
		ADS_SDATAC();
		HAL_Delay(100);
	#else
		//ADS_STOP();
		ADS_STOP(EEG_CS_Pin);
		HAL_Delay(100);
	#endif

	if(state)
	{
//		ADS_WREG(CH2SET,0x40);	
		ADS_WREG(CH1SET,0x40,EEG_CS_Pin);	
	  HAL_Delay(10);
	}
	else
	{
//		ADS_WREG(CH2SET,0xF0);
		ADS_WREG(CH1SET,0xF0,EEG_CS_Pin);	
		HAL_Delay(10);
	}
	HAL_Delay(1000);
	#if RDATAC
		ADS_RDATAC();            // enter Read Data Continuous mode
		HAL_Delay(100);
	#else
		ADS_START(EEG_CS_Pin); //??????????
		HAL_Delay(100);
	#endif
	
}

//心电采集控制，state=1: 开启通道1 state=0：关闭通道1
void start_ch1_sample(bool state)
{
	#if RDATAC
		ADS_SDATAC();
		HAL_Delay(100);
	#else
		ADS_STOP(ECG_CS_Pin);
		HAL_Delay(100);
	#endif

	if(state)
	{
		ADS_WREG(CH1SET,0x40,ECG_CS_Pin);
	  HAL_Delay(10);
	}
	else
	{
		ADS_WREG(CH1SET,0xF0,ECG_CS_Pin);
		HAL_Delay(10);
	}
	HAL_Delay(1000);
	#if RDATAC
		ADS_RDATAC();            // enter Read Data Continuous mode
		HAL_Delay(100);
	#else
		ADS_START(ECG_CS_Pin); //??????????
		HAL_Delay(100);
	#endif
}



