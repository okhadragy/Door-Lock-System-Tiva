#ifndef Common_Macros
#define Common_Macros

#define SET_BIT(REG,BIT) ((REG)|=(1U<<BIT))
#define CLEAR_BIT(REG,BIT) ((REG) &= ~(1U << (BIT)))
#define TOGGLE_BIT(REG,BIT) ((REG)) ^= (1U <<(BIT))
#define READ_BIT(REG,BIT) (((REG)>>(BIT))& 1U )
#endif