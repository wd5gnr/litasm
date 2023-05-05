#include "litasm.h"

class litasm1802: public litasm
{
 public:
  litasm1802();
  unsigned int pass;
  unsigned int pc;
 protected:
  unsigned genasm(unsigned pass);
};


litasm1802 asm1802;  // This is the one object we use

litasm *asmobj_create(void)  // tell the main program about us
{
  return &asm1802;
}

// Initial setup
litasm1802::litasm1802()
    {
      memsize=0x10000;  // 64k is cheap now
      psize=8;
      begin=0;
      pass=0;
      ary=new uint8_t[memsize];
    }


// Start a program
extern void Program(void);


// Manage the two passes
unsigned litasm1802::genasm(unsigned _pass)    
  {
    pass=_pass;
    err=0;
    end=0;
    pc=0;
    Program();
    return 0;
  };




// Set a byte into the array
static void __setbyte(uint8_t b)
{
  asm1802.ary[asm1802.pc++]=b;
}


// Check a register value
static uint8_t regcheck(uint8_t r)
{
  // todo: throw error if >15 but for now...
  return r&0xF;
}

// Set the origin which is the lowest address in the program
void Origin(uint16_t add)
{
  asm1802.pc=add;
  asm1802.begin=add;
}

void Location(uint16_t add)  // like orgin but don't set the start of memory (so location>=Origin)
{
  asm1802.pc=add;
}

// Resevere byte or bytes.
void Byte(uint8_t data=0, uint8_t count=1)
{
  while (count--) __setbyte(data);
}


// Todo: Need a String() to put a constant string of bytes in, too


// Define a lable
void Define_Label(const std::string s)
{
  asm1802.symtab[s]=asm1802.pc;
}

// Deref a label
uint16_t Label(const std::string s)
{
  return asm1802.symtab[s];  // todo: better error when undefined label
}

// Get the high and low parts of a label
uint16_t HIGH(const std::string s) { return Label(s)>>8; }
uint16_t LOW(const std::string s) { return Label(s)&0xFF; }

// Real Instructions; some have counts and things where it makes sense

void Idle(void) { __setbyte(0); }
void Load_D_From_Reg_Address(uint8_t reg)
{
  reg=regcheck(reg);
  if (reg==0) return; // todo: set error
  __setbyte(reg);
}
void Inc_Register(uint8_t reg)
{
  reg=regcheck(reg);
  __setbyte(0x10+reg);
}
void Dec_Register(uint8_t reg)
{
  reg=regcheck(reg);
  __setbyte(0x20+reg);
}

void Branch(uint16_t add, unsigned extra=0)
{
  if ((asm1802.pc & 0xFF00)!=(add&0xFF00)) ; // todo error here
  __setbyte(0x30+extra);
  __setbyte(add&0xFF);
}

void Branch_On_Q(uint16_t add) { Branch(add,1); }
void Branch_On_Zero(uint16_t add) { Branch(add,2); }
void Branch_On_Equal(uint16_t add) { Branch_On_Zero(add); }
void Branch_On_Carry(uint16_t add) { Branch(add,3); }
void Branch_On_Flag(uint16_t add, uint8_t flag) { Branch(add,4+((flag-1)&3)); }  // flag 1..4
void Skip(void) { __setbyte(0x38); }
void Branch_On_NoQ(uint16_t add) { Branch(add,9); }
void Branch_On_NoZero(uint16_t add) { Branch(add,0xA); }
void Branch_On_NotEqual(uint16_t add) { Branch_On_NoZero(add); }
void Branch_On_NoCarry(uint16_t add) { Branch(add,0xB); }
void Branch_On_NoFlag(uint16_t add, uint8_t flag) { Branch(add,0xC+((flag-1)&3)); }  // flag 1..4

void Load_D_From_Reg_Address_Advance(uint8_t reg)
{
  reg=regcheck(reg);
  __setbyte(0x40+reg);
}

void Store_D_To_Reg_Address(uint8_t reg)
{
  reg=regcheck(reg);
  __setbyte(0x50+reg);
}

void Increment_X(void) { __setbyte(0x60); }
void Output_Mem_RX_Incr(uint8_t port) { __setbyte(0x60+(port&0x7)); }
void Input_Mem_RX(uint8_t port) { __setbyte(0x69+(port&0x7)); }

void Return(uint8_t disable=0) { __setbyte(0x70+(disable&1)); }
void Return_Disable_Int(void) { Return(1); }
void Load_D_From_RX_Advance(void) { __setbyte(0x72); }
void Store_D_To_RX_Decr(void) { __setbyte(0x73); }
void Add_With_Carry(void) { __setbyte(0x74); }
void Subtract_Mem_D_Borrow(void) { __setbyte(0x75); }
void Shift_D_Right_Carry(void) { __setbyte(0x76); }
void Subtract_D_Mem_Borrow(void) { __setbyte(0x77); }
void Save_T_Mem(void) { __setbyte(0x78); }
void Push_XP_R2Stack(void) { __setbyte(0x79); }

void QLed(int onoff)
{
  __setbyte(0x7A+(onoff&1));
}

void QOn(void) { QLed(1); }
void QOff(void) { QLed(0); }
void Add_D_Imm_Carry(uint8_t imm) { __setbyte(0x7c); __setbyte(imm);  }
void Subtract_Imm_D_Borrow(uint8_t imm) { __setbyte(0x7d);  __setbyte(imm); } 
void Shift_D_Left_Carry(void) { __setbyte(0x7e); }
void Subtract_D_Imm_Borrow(uint8_t imm) { __setbyte(0x7f); __setbyte(imm); }  


void Get_Low_Register(uint8_t reg)
{
  reg=regcheck(reg);
  __setbyte(0x80+reg);
}

void Get_High_Register(uint8_t reg)
{
  reg=regcheck(reg);
  __setbyte(0x90+reg);
}

void Put_Low_Register(uint8_t reg)
{
  reg=regcheck(reg);
  __setbyte(0xA0+reg);
}

void Put_High_Register(uint8_t reg)
{
  reg=regcheck(reg);
  __setbyte(0xB0+reg);
}

void Jump(uint16_t label,uint8_t extra=0)
{
  __setbyte(0xC0+extra);
  __setbyte(label>>8);  
  __setbyte(label&0xFF);
}

void Jump_On_Q(uint16_t label) { Jump(label,1); }
void Jump_On_Zero(uint16_t label) { Jump(label,2); }
void Jump_On_Equal(uint16_t label) { Jump_On_Zero(label); }
void Jump_On_Carry(uint16_t label) { Jump(label,3); }
void NOP(uint8_t count=1) {while (count--)  __setbyte(0xc4); }
void Long_Skip_On_NoQ(void) { __setbyte(0xc5); }
void Long_Skip_On_NoZero(void) { __setbyte(0xc6); }
void Long_Skip_On_NoCarry(void) { __setbyte(0xc7); }
void Long_Skip(void) { __setbyte(0xc8); }
void Jump_On_NoQ(uint16_t label) { Jump(label,9); }
void Jump_On_NoZero(uint16_t label) { Jump(label,0xA); }
void Jump_On_NotEqual(uint16_t label) { Jump_On_NoZero(label); }
void Jump_On_NoCarry(uint16_t label) { Jump(label,0xB); }
void Long_Skip_On_IntEnable(void) { __setbyte(0xcc); }
void Long_Skip_On_Q(void) { __setbyte(0xcd); }
void Long_Skip_On_Zero(void) { __setbyte(0xce); }
void Long_Skip_On_Carry(void) { __setbyte(0xcf); }


void Set_PC_To_Register(uint8_t reg)
{
  reg=regcheck(reg);
  __setbyte(0xD0+reg);
}

void Set_X_To_Register(uint8_t reg)
{
  reg=regcheck(reg);
  __setbyte(0xE0+reg);
}

void Load_D_From_RX(void) { __setbyte(0xF0); }
void Or_D_X(void) { __setbyte(0xF1); }
void And_D_X(void) { __setbyte(0xF2); }
void Xor_D_X(void) { __setbyte(0xF3); }
void Add_D_X(void) { __setbyte(0xF4); }
void Subtract_X_D(void) { __setbyte(0xF5); }
void Shift_D_Right(void) { __setbyte(0xF6); }
void Subtract_D_X(void) { __setbyte(0xf7); }
void Load_D_Imm(uint8_t imm) { __setbyte(0xf8); __setbyte(imm); }
void Or_D_Imm(uint8_t imm) { __setbyte(0xF9); __setbyte(imm); }
void And_D_Imm(uint8_t imm) { __setbyte(0xFA); __setbyte(imm); }
void XOR_D_Imm(uint8_t imm) { __setbyte(0xFB); __setbyte(imm); }
void Add_D_Imm(uint8_t imm) { __setbyte(0xFC); __setbyte(imm); }
void Subtract_Imm_D(uint8_t imm) { __setbyte(0xFD); __setbyte(imm); }
void Shift_D_Left(void) { __setbyte(0xFE); }
void Subtract_D_Imm(uint8_t imm) { __setbyte(0xFF); __setbyte(imm); }


// End a program with an optional start address
void End_Program(uint16_t start_address=0x0000)
{
  asm1802.end=asm1802.pc-1;
  asm1802.begin=start_address;
}


// useful quasi instruction
void Load_R_Label(uint8_t reg,const std::string s)
{
  Load_D_Imm(HIGH(s));
  Put_High_Register(reg);
  Load_D_Imm(LOW(s));
  Put_Low_Register(reg);
}

