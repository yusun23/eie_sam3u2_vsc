/*!*********************************************************************************************************************
@file user_app1.c                                                                
@brief User's tasks / applications are written here.  This description
should be replaced by something specific to the task.

----------------------------------------------------------------------------------------------------------------------
To start a new task using this user_app1 as a template:
 1. Copy both user_app1.c and user_app1.h to the Application directory
 2. Rename the files yournewtaskname.c and yournewtaskname.h
 3. Add yournewtaskname.c and yournewtaskname.h to the Application Include and Source groups in the IAR project
 4. Use ctrl-h (make sure "Match Case" is checked) to find and replace all instances of "user_app1" with "yournewtaskname"
 5. Use ctrl-h to find and replace all instances of "UserApp1" with "YourNewTaskName"
 6. Use ctrl-h to find and replace all instances of "USER_APP1" with "YOUR_NEW_TASK_NAME"
 7. Add a call to YourNewTaskNameInitialize() in the init section of main
 8. Add a call to YourNewTaskNameRunActiveState() in the Super Loop section of main
 9. Update yournewtaskname.h per the instructions at the top of yournewtaskname.h
10. Delete this text (between the dashed lines) and update the Description below to describe your task
----------------------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------------------------
GLOBALS
- NONE

CONSTANTS
- NONE

TYPES
- NONE

PUBLIC FUNCTIONS
- NONE

PROTECTED FUNCTIONS
- void UserApp1Initialize(void)
- void UserApp1RunActiveState(void)


**********************************************************************************************************************/

#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_<type>UserApp1"
***********************************************************************************************************************/
/* New variables */
volatile u32 G_u32UserApp1Flags;                          /*!< @brief Global state flags */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemTime1ms;                   /*!< @brief From main.c */
extern volatile u32 G_u32SystemTime1s;                    /*!< @brief From main.c */
extern volatile u32 G_u32SystemFlags;                     /*!< @brief From main.c */
extern volatile u32 G_u32ApplicationFlags;                /*!< @brief From main.c */

extern u8 G_au8DebugScanfBuffer[DEBUG_SCANF_BUFFER_SIZE]; // From debug.c
extern u8 G_u8DebugScanfCharCount;                        // From debug.c
/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp1_<type>" and be declared as static.
***********************************************************************************************************************/
static fnCode_type UserApp1_pfStateMachine;               /*!< @brief The state machine function pointer */
//static u32 UserApp1_u32Timeout;                           /*!< @brief Timeout counter used across states */
const u8 startLogo[U8_LCD_IMAGE_ROW_SIZE_25PX][U8_LCD_IMAGE_COL_BYTES_25PX] = {
  {0xFF, 0xFF, 0xFF, 0x01},
  {0x1F, 0x00, 0xE0, 0x01},
  {0xEF, 0xFF, 0xDF, 0x01},
  {0x2F, 0x84, 0xD0, 0x01},
  {0x2F, 0x84, 0xD0, 0x01},
  {0x2F, 0x84, 0xD0, 0x01},
  {0x2F, 0x84, 0xD0, 0x01},
  {0xEF, 0xFF, 0xDF, 0x01},
  {0x1F, 0x80, 0xD0, 0x01},
  {0xFF, 0xBF, 0xD0, 0x01},
  {0xFF, 0xBF, 0xD0, 0x01},
  {0xFF, 0xBF, 0xD0, 0x01},
  {0xFF, 0xBF, 0xDF, 0x01},
  {0xFF, 0x01, 0xE0, 0x01},
  {0xFF, 0xFD, 0xFE, 0x01},
  {0xFF, 0x85, 0xFE, 0x01},
  {0xFF, 0x85, 0xFE, 0x01},
  {0xFF, 0x85, 0xFE, 0x01},
  {0x1F, 0x84, 0xE0, 0x01},
  {0xEF, 0xFF, 0xDF, 0x01},
  {0x2F, 0x84, 0xD0, 0x01},
  {0x2F, 0x84, 0xD0, 0x01},
  {0x2F, 0x84, 0xD0, 0x01},
  {0x2F, 0x84, 0xD0, 0x01},
  {0xEF, 0xFF, 0xDF, 0x01}
  };  
  
const u8 LongBlock[U8_LCD_IMAGE_ROW_PIXEL_BLOCK1][U8_LCD_IMAGE_COLUMN_PIXEL_BLOCK3PX] = {
  {0xFF, 0xFF, 0x1F},
  {0x21, 0x84, 0x10},
  {0x21, 0x84, 0x10},
  {0x21, 0x84, 0x10},
  {0x21, 0x84, 0x10},
  {0xFF, 0xFF, 0x1F}
  };

const u8 LBlock [U8_LCD_IMAGE_ROW_PIXEL_BLOCK3][U8_LCD_IMAGE_COLUMN_PIXEL_BLOCK2PX] = {
  {0x3F, 0x00},
  {0x21, 0x00},
  {0x21, 0x00},
  {0x21, 0x00},
  {0x21, 0x00},
  {0x3F, 0x00},
  {0x21, 0x00},
  {0x21, 0x00},
  {0x21, 0x00},
  {0x21, 0x00},
  {0xFF, 0x07},
  {0x21, 0x04},
  {0x21, 0x04},
  {0x21, 0x04},
  {0x21, 0x04},
  {0xFF, 0x07}
  };

const u8 TBlock[U8_LCD_IMAGE_ROW_PIXEL_BLOCK2][U8_LCD_IMAGE_COLUMN_PIXEL_BLOCK2PX] = {
  {0xE0, 0x07},
  {0x20, 0x04},
  {0x20, 0x04},
  {0x20, 0x04},
  {0x20, 0x04},
  {0xFF, 0xFF},
  {0x21, 0x84},
  {0x21, 0x84},
  {0x21, 0x84},
  {0x21, 0x84},
  {0xFF, 0xFF}
  };    
    
const u8 squareBlock[U8_LCD_IMAGE_ROW_PIXEL_BLOCK2][U8_LCD_IMAGE_COLUMN_PIXEL_BLOCK2PX] = {
  {0xFF, 0x07},
  {0x21, 0x04},
  {0x21, 0x04},
  {0x21, 0x04},
  {0x21, 0x04},
  {0xFF, 0x07},
  {0x21, 0x04},
  {0x21, 0x04},
  {0x21, 0x04},
  {0x21, 0x04},
  {0xFF, 0x07}
  };      

const u8 JBlock[U8_LCD_IMAGE_ROW_PIXEL_BLOCK3][U8_LCD_IMAGE_COLUMN_PIXEL_BLOCK2PX] = {
  {0xE0, 0x07},
  {0x20, 0x04},
  {0x20, 0x04},
  {0x20, 0x04},
  {0x20, 0x04},
  {0xE0, 0x07},
  {0x20, 0x04},
  {0x20, 0x04},
  {0x20, 0x04},
  {0x20, 0x04},
  {0xFF, 0x07},
  {0x21, 0x04},
  {0x21, 0x04},
  {0x21, 0x04},
  {0x21, 0x04},
  {0xFF, 0x07}
  };
      
const u8 WeirdShapeLeft[U8_LCD_IMAGE_ROW_PIXEL_BLOCK3][U8_LCD_IMAGE_COLUMN_PIXEL_BLOCK2PX] = {
  {0x3F, 0x00},
  {0x21, 0x00},
  {0x21, 0x00},
  {0x21, 0x00},
  {0x21, 0x00},
  {0xFF, 0x07},
  {0x21, 0x04},
  {0x21, 0x04},
  {0x21, 0x04},
  {0x21, 0x04},
  {0xFF, 0x07},
  {0x20, 0x04},
  {0x20, 0x04},
  {0x20, 0x04},
  {0x20, 0x04},
  {0xE0, 0x07}
  };

const u8 WeirdBlockRight[U8_LCD_IMAGE_ROW_PIXEL_BLOCK3][U8_LCD_IMAGE_COLUMN_PIXEL_BLOCK2PX] = {
  {0xE0, 0x07},
  {0x20, 0x04},
  {0x20, 0x04},
  {0x20, 0x04},
  {0x20, 0x04},
  {0xFF, 0x07},
  {0x21, 0x04},
  {0x21, 0x04},
  {0x21, 0x04},
  {0x21, 0x04},
  {0xFF, 0x07},
  {0x21, 0x00},
  {0x21, 0x00},
  {0x21, 0x00},
  {0x21, 0x00},
  {0x3F, 0x00}
  };
  
  static u16 memoryImage[128][64] = {};
        
/**********************************************************************************************************************
Function Definitions
**********************************************************************************************************************/
int random(int count);
void assignment(int blockType, PixelBlockType *address, u8 **destination);
/*--------------------------------------------------------------------------------------------------------------------*/
/*! @publicsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------*/
/*! @protectedsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/*!--------------------------------------------------------------------------------------------------------------------
@fn void UserApp1Initialize(void)

@brief
Initializes the State Machine and its variables.

Should only be called once in main init section.

Requires:
- NONE

Promises:
- NONE

*/
void UserApp1Initialize(void)
{
  const u8 Welcome[] = {"Welcome to Tetris"};
  const static PixelAddressType startLocation = {5, 12};
  LcdClearScreen();
  LcdLoadString(Welcome, LCD_FONT_SMALL, &startLocation);
  PixelBlockType startImage;
  startImage.u16RowStart = 21;
  startImage.u16ColumnStart = 53;
  startImage.u16RowSize = 25;
  startImage.u16ColumnSize = 25;
  LcdLoadBitmap(&startLogo[0][0], &startImage);
  /* If good initialization, set state to Idle */
  if( 1 )
  {
    UserApp1_pfStateMachine = UserApp1SM_Idle;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    UserApp1_pfStateMachine = UserApp1SM_Error;
  }

} /* end UserApp1Initialize() */

  
/*!----------------------------------------------------------------------------------------------------------------------
@fn void UserApp1RunActiveState(void)

@brief Selects and runs one iteration of the current state in the state machine.

All state machines have a TOTAL of 1ms to execute, so on average n state machines
may take 1ms / n to execute.

Requires:
- State machine function pointer points at current state

Promises:
- Calls the function to pointed by the state machine function pointer

*/
void UserApp1RunActiveState(void)
{
  UserApp1_pfStateMachine();

} /* end UserApp1RunActiveState */


/*------------------------------------------------------------------------------------------------------------------*/
/*! @privatesection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/


/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/
/*-------------------------------------------------------------------------------------------------------------------*/
/* What does this state do? */
static void UserApp1SM_Idle(void)
{
    static int count = 0;
    PixelBlockType location;
    u8 spawn = 0;
    u8 *address = NULL;  // Initialize to NULL

    if (count == 2147483647)
        count = 0;
    count++;
    spawn = random(count);
    assignment(spawn, &location, &address);
    if (WasButtonPressed(BUTTON0))
    {
        ButtonAcknowledge(BUTTON0);
        LcdClearScreen();
        LcdLoadBitmap(address, &location);
    }
} /* end UserApp1SM_Idle() */
     

/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp1SM_Error(void)          
{
  
} /* end UserApp1SM_Error() */

int random(int count)
{
  return count % 7;
}

void assignment(int blockType, PixelBlockType *address, u8 **destination)
{
    u8 rowSize = 0;
    u8 columnSize = 0;
    
    switch (blockType)
    {
      case 0:
        rowSize = U8_LCD_IMAGE_ROW_PIXEL_BLOCK1;
        columnSize = U8_LCD_IMAGE_COLUMN_PIXEL_BLOCK4;
        *destination = &LongBlock[0][0];
        break;
      case 1:
        rowSize = U8_LCD_IMAGE_ROW_PIXEL_BLOCK3;
        columnSize = U8_LCD_IMAGE_COLUMN_PIXEL_BLOCK2;
        *destination = &LBlock[0][0];
        break;
      case 2:
        rowSize = U8_LCD_IMAGE_ROW_PIXEL_BLOCK3;
        columnSize = U8_LCD_IMAGE_COLUMN_PIXEL_BLOCK2;
        *destination = &JBlock[0][0];
        break;
      case 3:
        rowSize = U8_LCD_IMAGE_ROW_PIXEL_BLOCK3;
        columnSize = U8_LCD_IMAGE_COLUMN_PIXEL_BLOCK2;
        *destination = &WeirdBlockRight[0][0];
        break;
      case 4:
        rowSize = U8_LCD_IMAGE_ROW_PIXEL_BLOCK3;
        columnSize = U8_LCD_IMAGE_COLUMN_PIXEL_BLOCK2;
        *destination = &WeirdShapeLeft[0][0];
        break;
      case 5:
        rowSize = U8_LCD_IMAGE_ROW_PIXEL_BLOCK2;
        columnSize = U8_LCD_IMAGE_COLUMN_PIXEL_BLOCK2;
        *destination = &squareBlock[0][0];
        break;
      case 6:
        rowSize = U8_LCD_IMAGE_ROW_PIXEL_BLOCK2;
        columnSize = U8_LCD_IMAGE_COLUMN_PIXEL_BLOCK3;
        *destination = &TBlock[0][0];
        break;
    }
    address->u16RowStart = 25;
    address->u16ColumnStart = 0;
    address->u16RowSize = rowSize;
    address->u16ColumnSize = columnSize;
}


/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
