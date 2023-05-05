// Simple 1802 Literate Program

#include "lit1802.h"

#define ON 1
#define OFF 0

#define DELAYPC 9  // delay subroutine
#define DELAYR 8   // delay count register
#define MAINPC 3   // Main routine PC
#define RX 7       // RX value
#define DELAYVAL 0xFF  // time to delay (0-255)

void Program(void)
{
  Origin(0x0);
  // Blinky light program
  // Main:
  Define_Label("Main");
  // Force R3 as PC just in case
  Load_R_Label(MAINPC,"R3Go");
  Set_PC_To_Register(MAINPC);
  // Here we are P=3
  // R3Go:
  Define_Label("R3Go");
  // Set R9 to delay routine (default PC=0)
  Load_R_Label(DELAYPC,"Delay");
  // Set RX=7 at memory 00F0
  Load_R_Label(RX,"Stack");
  Set_X_To_Register(RX);
  Load_D_Imm(0);
  Store_D_To_Reg_Address(RX);

  // Loop:
  Define_Label("Loop");
  Output_Mem_RX_Incr(4);       // write count to LED
  Dec_Register(RX);            // adjust 
  // bump count
  Load_D_From_Reg_Address(RX);
  Add_D_Imm(1);
  Store_D_To_Reg_Address(RX);
  // led flash sequence
  QLed(ON);
  Set_PC_To_Register(DELAYPC);
  QLed(OFF);
  Set_PC_To_Register(DELAYPC);
  Branch(Label("Loop"));


  // Delay subroutine
  // Delay:  
  Define_Label("Delay");
  Load_D_Imm(0);
  Put_High_Register(DELAYR);
  Load_D_Imm(DELAYVAL);
  Put_Low_Register(DELAYR);
  // Delay1:
  Define_Label("Delay1");
  Get_Low_Register(DELAYR);
  Branch_On_NoZero(Label("DelayCont"));
  Set_PC_To_Register(MAINPC);  // return
  Branch(Label("Delay")); // for next time

  // DelayCont:
  Define_Label("DelayCont");  // keep going
  Dec_Register(DELAYR);
  NOP(10);
  Branch(Label("Delay1"));   // note... could define BRANCH as _BRANCH and then #define Branch(l) _BRANCH(Label(l)) if you like... 

  Location(0xF0);  // storage for RX
  // Stack:
  Define_Label("Stack");
  Byte();

  End_Program(Label("Main"));  // End of program
}
