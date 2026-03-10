#ifndef __FUN_H
#define __FUN_H

uint8_t ButtonScan(void);

void LedControl(uint8_t Ledx, uint8_t LedState);
void LedClose(void);
void UserLcdInit1(void);

void LcdShowData(void);

void LcdShowPara(void);

void ProcessData(void);
void OutputCharge(void);

extern uint8_t ChargeFlag;
extern uint8_t NumOfIDLE;
extern uint8_t FlashFlag;
extern double CNBR;
extern double VNBR;
extern uint8_t Rx;
#endif
