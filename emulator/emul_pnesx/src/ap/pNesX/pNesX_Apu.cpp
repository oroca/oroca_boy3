/*===================================================================*/
/*                                                                   */
/*  pNesX_Apu.cpp : NES APU emulation (for Nucleo)                   */
/*                                                                   */
/*  2016/1/20  Racoon                                                */
/*                                                                   */
/*===================================================================*/

#include "ap.h"
#include "pNesX.h"
#include "pNesX_System.h"

//====================
// DAC
//====================
WORD TrDac, P1Dac, P2Dac, NsDac, MixDac;

#define DACFreq 24000
//#define DACFreq 11025


// Pulse clock(8 steps) = 1789773 / 2 / DACFreq * 1000
#define PuClock 37287
//#define PuClock 81169

// Triangle clock(32 steps) = 1789773 / DACFreq * 1000
#define TrClock 74574
//#define TrClock 162338

//====================
// DAC Timer
//====================
#define TIMx                TIM1
#define TIMx_CLK_ENABLE     __HAL_RCC_TIM1_CLK_ENABLE
#define TIMx_IRQn           TIM1_IRQn
#define TIMx_IRQHandler     TIM1_IRQHandler
TIM_HandleTypeDef TimHandle;

//====================
// Audio channel variables
//====================

const BYTE LengthTable[] = {10, 254, 20,  2, 40,  4, 80,  6, 160,  8, 60, 10, 14, 12, 26, 14,
                            12, 16, 24, 18, 48, 20, 96, 22, 192, 24, 72, 26, 16, 28, 32, 30};

// Minimum Volume
#define MV 4096
const WORD Pulse_Table[4][8] = { 0,MV, 0, 0, 0, 0, 0, 0,
                                 0,MV,MV, 0, 0, 0, 0, 0,
                                 0,MV,MV,MV,MV, 0, 0, 0,
                                MV, 0, 0,MV,MV,MV,MV,MV};
// Pulse1
bool P1Enable;
int P1Timer;
int P1WaveLength;
int P1WavePeriod;
int P1Lapse;
int P1Duty;
int P1Vol;
int P1LengthCounter;
int P1EnvSeq;
int P1EnvCnt;
int P1Sweep;
int P1SwCnt;
int P1SwLevel;
bool P1SwReloadFlag;
bool P1SwOverflow;

// Pulse2
bool P2Enable;
int P2Timer;
int P2WaveLength;
int P2WavePeriod;
int P2Lapse;
int P2Duty;
int P2Vol;
int P2LengthCounter;
int P2EnvSeq;
int P2EnvCnt;
int P2Sweep;
int P2SwCnt;
int P2SwLevel;
bool P2SwReloadFlag;
bool P2SwOverflow;
    
// Triangle                   
bool TrEnable;
int TrTimer;
const WORD Triangle_Table[] = {61440,57344,53248,49152,45056,40960,36864,32768,28672,24576,20480,16384,12288,8192,4096,0,
                               0,4096,8192,12288,16384,20480,24576,28672,32768,36864,40960,45056,49152,53248,57344,61440};
bool LinearCounterReloadFlag;
int LinearCounter;
int TrLengthCounter;
int TrWaveLength;
int TrWavePeriod;
int TrLapse;

// Noise
bool NsEnable;
const WORD Noise_Freq[] = {0,0,0,0,0,0,12000,11186,8860,7046,4710,3523,2349,1762,880,440};
int NsFreq;
int NsSum;
int NsVol;
int NsLengthCounter;
int NsEnvSeq;
int NsEnvCnt;

// DMC
int DmOutLevel;

static uint8_t sound_buf[1024];
static uint8_t sound_buf_q_mem[256];
//static qbuffer_node_t sound_buf_q;
static uint16_t sound_vol = 20;
static volatile bool play_sound_flag = false;

void ApuPlayStart(void);


/*-------------------------------------------------------------------*/
/*  DAC Timer callback                                               */
/*-------------------------------------------------------------------*/

/*-------------------------------------------------------------------*/
/*  Randomize value (Noise channel)                                  */
/*-------------------------------------------------------------------*/
WORD ShiftReg = 1;

WORD Rand96()
{
    ShiftReg |= ((ShiftReg ^ (ShiftReg >> 6)) & 1) << 15;
    ShiftReg >>= 1;
    return (ShiftReg & 1) ? MV : 0;    
}

WORD Rand32k()
{
    ShiftReg |= ((ShiftReg ^ (ShiftReg >> 1)) & 1) << 15;
    ShiftReg >>= 1;
    return (ShiftReg & 1) ? MV : 0;    
}

/*-------------------------------------------------------------------*/
/*  DAC Timer Interrup                                               */
/*-------------------------------------------------------------------*/
void pNesX_ISR(void)
{
    // Pulse1
    if (P1Enable && P1LengthCounter > 0 && !P1SwOverflow && P1Timer > 8)
    {
        P1Lapse += PuClock; 
        P1Lapse %= P1WavePeriod;
        P1Dac = Pulse_Table[P1Duty][P1Lapse / P1WaveLength] * P1Vol;           
    }

    // Pulse2
    if (P2Enable && P2LengthCounter > 0 && !P2SwOverflow && P2Timer > 8)
    {
        P2Lapse += PuClock; 
        P2Lapse %= P2WavePeriod;
        P2Dac = Pulse_Table[P2Duty][P2Lapse / P2WaveLength] * P2Vol;   
    }
    
    // Triangle
    if (TrEnable && LinearCounter > 0 && TrLengthCounter > 0)
    {
        TrLapse += TrClock; 
        TrLapse %= TrWavePeriod;
        TrDac = Triangle_Table[TrLapse / TrWaveLength];        
        TrDac -= (TrDac * DmOutLevel / 295);
    }
    
    // Noise
    if (NsEnable && NsLengthCounter > 0)
    {
        NsSum += NsFreq;
        if (NsSum >= DACFreq)
        {
            NsSum %= DACFreq;

            NsDac = ((APU_Reg[0xe] & 0x80) ? Rand96() : Rand32k()) * NsVol;
            NsDac -= (NsDac * DmOutLevel / 295);            
        }        
    }
        
    // Mixing
    MixDac = (P1Dac + P2Dac + TrDac + NsDac) / (4);
        
    //dacPutch(map(MixDac>>8, 0, 255, 0, sound_vol));
    speakerPutch(MixDac>>8);
}


void ApuAdjustVolume(int8_t step)
{
  sound_vol += step;
  sound_vol = constrain(sound_vol, 0, 100);

  //speakerSetVolume(sound_vol);
}
/*-------------------------------------------------------------------*/
/*  Apu Initialize Function                                          */
/*-------------------------------------------------------------------*/
void ApuInit()
{
  timerSetPeriod(_DEF_TIMER1, 1000000/DACFreq);
  timerAttachInterrupt(_DEF_TIMER1, pNesX_ISR);
  timerStart(_DEF_TIMER1);

  //speakerSetVolume(sound_vol);
  speakerStart(DACFreq);
}

/*-------------------------------------------------------------------*/
/*  Apu Mute                                                         */
/*-------------------------------------------------------------------*/
void ApuMute(bool mute)
{
  if (mute)
  {
    speakerDisable();
  }
  else
  {
    speakerEnable();
  }
}
      
/*-------------------------------------------------------------------*/
/*  Apu Write Function                                               */
/*-------------------------------------------------------------------*/
void ApuWrite( WORD wAddr, BYTE byData )
{
  APU_Reg[ wAddr ] = byData;

  switch( wAddr )
  {
    //====================
    // Pulse1
    //====================
    case 0:
      P1Duty = (byData & 0xc0) >> 6;
    
      if (byData & 0x10)
      {
        // constant volume
        P1Vol = byData & 0xf;
      }
      else
      {
        // envelope on
        P1Vol = 15;
        P1EnvCnt = (byData & 0xf) + 1;
      }
      break;

    case 1:
      P1SwReloadFlag = true;
      P1SwLevel = (byData & 7);
      break;
  
    case 2:
    case 3:
      P1Timer = APU_Reg[3] & 7;
      P1Timer = (P1Timer << 8) | APU_Reg[2];
      P1WaveLength = (P1Timer+1) * 1000;
      P1WavePeriod = P1WaveLength * 8;               

      if (wAddr == 3)
      {
        P1LengthCounter = LengthTable[(byData & 0xf8) >> 3];
    
        // Reset sequencer
        P1Lapse = 0;
        P1EnvSeq = P1EnvCnt;
        P1Sweep = 0;
        P1SwCnt = 0;
        P1SwOverflow = false;
      }
      break;
      
    //====================
    // Pulse2
    //====================
    case 4:
      P2Duty = (byData & 0xc0) >> 6;
    
      if (byData & 0x10)
      {
        // constant volume
        P2Vol = byData & 0xf;
      }
      else
      {
        // envelope on
        P2Vol = 15;
        P2EnvCnt = (byData & 0xf) + 1;
      }
      break;
      
    case 5:
      P2SwReloadFlag = true;
      P2SwLevel = (byData & 7);
      break;
  
    case 6:
    case 7:
      P2Timer = APU_Reg[7] & 7;
      P2Timer = (P2Timer << 8) | APU_Reg[6];
      P2WaveLength = (P2Timer+1) * 1000;
      P2WavePeriod = P2WaveLength * 8;         
      if (wAddr == 7)
      {
        P2LengthCounter = LengthTable[(byData & 0xf8) >> 3];
    
        // Reset sequencer
        P2Lapse = 0;
        P2EnvSeq = P2EnvCnt;
        P2Sweep = 0;
        P2SwCnt = 0;
        P2SwOverflow = false;
      }
      break;
      
    //====================
    // Triangle
    //====================
    case 0x8:
      LinearCounterReloadFlag = true;
      break;
      
    case 0xA:
    case 0xB:
      TrTimer = APU_Reg[0xB] & 7;
      TrTimer = (TrTimer << 8) | APU_Reg[0xA];
      TrWaveLength = TrTimer * 1000;
      TrWavePeriod = TrWaveLength * 32;          
      
      if (wAddr == 0xB)
      {
        TrLengthCounter = LengthTable[(byData & 0xf8) >> 3];
        LinearCounterReloadFlag = true;
      }
      break;
      
    //====================
    // Noise
    //====================
    case 0xC:
      if (byData & 0x10)
      {
        // constant volume
        NsVol = byData & 0xf;
      }
      else
      {
        // envelope on
        NsVol = 15;
        NsEnvCnt = (byData & 0xf) + 1;
      }
      break;
        
    case 0xF:
      NsLengthCounter = LengthTable[(byData & 0xf8) >> 3];
    
      // Reset sequencer
      NsEnvSeq = NsEnvCnt;
      break;

    case 0xE:
      NsFreq = Noise_Freq[byData & 0xf];
      break;
  
    //====================
    // DMC(PCM)
    //====================
    case 0x11:
      DmOutLevel = byData & 0x7f;
      break;
      
    //====================
    // Control
    //====================
    case 0x15:
      if (byData & 1)
      {
        P1Enable = true;
      }
      else
      {
        P1Enable = false;
        P1LengthCounter = 0;
      }
              
      if (byData & 2)
      {
        P2Enable = true;
      }
      else
      {
        P2Enable = false;
        P2LengthCounter = 0;
      }
      
      if (byData & 4)
      {
        TrEnable = true;
      }
      else
      {
        TrEnable = false;
        TrLengthCounter = 0;
      }

      if (byData & 8)
      {
        NsEnable = true;
      }
      else
      {
        NsEnable = false;
        NsLengthCounter = 0;
      }
      break;
  } // Switch( wAddr ) 
}

/*-------------------------------------------------------------------*/
/*  240Hz Clock                                                      */
/*-------------------------------------------------------------------*/
void pNesX_ApuClk_240Hz()
{
  //====================
  // Pulse1 Envelope
  //====================
  if (!(APU_Reg[0] & 0x10) && P1EnvSeq > 0)
  {
    P1EnvSeq--;
    if (P1EnvSeq == 0)
    {
      if (P1Vol > 0)
      {
        --P1Vol;
        P1EnvSeq = P1EnvCnt;
      }
      else
      { 
        if (APU_Reg[0] & 0x20)
        {
          P1Vol = 15;
          P1EnvSeq = P1EnvCnt;
        }
      }
    }    
  }

  //====================
  // Pulse2 Envelope
  //====================
  if (!(APU_Reg[4] & 0x10) && P2EnvSeq > 0)
  {
    P2EnvSeq--;
    if (P2EnvSeq == 0)
    {
      if (P2Vol > 0)
      {
        --P2Vol;
        P2EnvSeq = P2EnvCnt;
      }
      else
      { 
        if (APU_Reg[4] & 0x20)
        {
          P2Vol = 15;
          P2EnvSeq = P2EnvCnt;
        }
      }
    }    
  }
    
  //====================
  // Triangle Linear Counter
  //====================
  if (LinearCounterReloadFlag)
  {
    LinearCounter = APU_Reg[0x8] & 0x7f;
  }
  else if (LinearCounter > 0)
  {
    LinearCounter--;
  }
  
  if (!(APU_Reg[0x8] & 0x80))
  {
    LinearCounterReloadFlag = false;
  }
  
  //====================
  // Noise Envelope
  //====================
  if (!(APU_Reg[0xc] & 0x10) && NsEnvSeq > 0)
  {
    NsEnvSeq--;
    if (NsEnvSeq == 0)
    {
      if (NsVol > 0)
      {
        --NsVol;
        NsEnvSeq = NsEnvCnt;
      }
      else
      { 
        if (APU_Reg[0xc] & 0x20)
        {
          NsVol = 15;
          NsEnvSeq = NsEnvCnt;
        }
      }
    }    
  }  
}

/*-------------------------------------------------------------------*/
/*  120Hz Clock                                                      */
/*-------------------------------------------------------------------*/
void pNesX_ApuClk_120Hz()
{
  //====================
  // Pulse1 Sweep
  //====================
  if (P1SwReloadFlag)
  {
    P1Sweep = ((APU_Reg[1] & 0x70) >> 4) + 1;
    P1SwCnt = 0;
    P1SwReloadFlag = false;    
  }
  else if (APU_Reg[1] & 0x80 && !P1SwOverflow)
  {
    P1SwCnt++;
    if (P1SwCnt == P1Sweep)
    {
      P1SwCnt = 0;
      
      int sweep = P1Timer >> P1SwLevel;
      int value = P1Timer;
      if (APU_Reg[1] & 8)
      {
        value = value - sweep - 1;
        if (value < 8)
        {
          P1SwOverflow = true;
          return;
        }
      }
      else
      {
        value = value + sweep;
        if (value > 0x7ff)
        {
          P1SwOverflow = true;
          return;
        }
      } 
      P1Timer = value;
      
      APU_Reg[3] &= 7;
      APU_Reg[3] |= value >> 8;
      APU_Reg[2] = value & 0xff;
       
      P1WaveLength = (value + 1) * 1000;
      P1WavePeriod = P1WaveLength * 8;         
    }
  }

  //====================
  // Pulse1 Sweep
  //====================  
  if (P2SwReloadFlag)
  {
    P2Sweep = ((APU_Reg[5] & 0x70) >> 4) + 1;
    P2SwCnt = 0;
    P2SwReloadFlag = false;    
  }
  else if (APU_Reg[5] & 0x80 && !P2SwOverflow)
  {
    P2SwCnt++;
    if (P2SwCnt == P2Sweep)
    {
      P2SwCnt = 0;
      
      int sweep = P2Timer >> P2SwLevel;
      int value = P2Timer;
      if (APU_Reg[5] & 8)
      {
        value = value - sweep;
        if (value < 8)
        {
          P2SwOverflow = true;
          return;
        }
      }
      else
      {
        value = value + sweep;
        if (value > 0x7ff)
        {
          P2SwOverflow = true;
          return;
        }
      } 
      
      P2Timer = value;
      
      APU_Reg[7] &= 7;
      APU_Reg[7] |= value >> 8;
      APU_Reg[6] = value & 0xff;
       
      P2WaveLength = (value + 1) * 1000;
      P2WavePeriod = P2WaveLength * 8;         
      
    }
  }
}

/*-------------------------------------------------------------------*/
/*  60Hz Clock                                                       */
/*-------------------------------------------------------------------*/
void pNesX_ApuClk_60Hz()
{
  // Pulse1
  if (!(APU_Reg[0] & 0x20) && P1LengthCounter > 0)
  {
    P1LengthCounter--;
  }

  // Pulse2
  if (!(APU_Reg[4] & 0x20) && P2LengthCounter > 0)
  {
    P2LengthCounter--;
  }

  // Triangle
  if (!(APU_Reg[0x8] & 0x80) && TrLengthCounter > 0)
  {
    TrLengthCounter--;
  }
  
  // Noise
  if (!(APU_Reg[0xc] & 0x20) && NsLengthCounter > 0)
  {
    NsLengthCounter--;
  }
  
}

/*-------------------------------------------------------------------*/
/*  Status                                                           */
/*-------------------------------------------------------------------*/
BYTE ApuRead( WORD wAddr )
{
  if (wAddr == 0x15)
  {
    return (P1LengthCounter > 0) | (P2LengthCounter > 0) << 1 | (TrLengthCounter > 0) << 2 | (NsLengthCounter > 0) << 3;
  }
  
  return APU_Reg[ wAddr ];  
}

