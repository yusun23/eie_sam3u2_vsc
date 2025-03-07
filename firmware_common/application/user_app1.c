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
// Pointers for specified pixel art of keys
static u8 *assignedFront = NULL;
static u8 *assignedBack = NULL;
// Images of different keys
extern const u8 startLogo[U8_LCD_IMAGE_ROW_SIZE_50PX][U8_LCD_IMAGE_COL_BYTES_50PX];
extern const u8 GKey[U8_LCD_IMAGE_ROW_SIZE_50PX][U8_LCD_IMAGE_COL_BYTES_50PX];
extern const u8 AKey[U8_LCD_IMAGE_ROW_SIZE_50PX][U8_LCD_IMAGE_COL_BYTES_50PX];
extern const u8 BKey[U8_LCD_IMAGE_ROW_SIZE_50PX][U8_LCD_IMAGE_COL_BYTES_50PX];
extern const u8 FKeyBack[U8_LCD_IMAGE_ROW_SIZE_50PX][U8_LCD_IMAGE_COL_BYTES_50PX];
extern const u8 FSharpKey[U8_LCD_IMAGE_ROW_SIZE_50PX][U8_LCD_IMAGE_COL_BYTES_50PX];
extern const u8 GSharpKey[U8_LCD_IMAGE_ROW_SIZE_50PX][U8_LCD_IMAGE_COL_BYTES_50PX];
extern const u8 ASharpKey[U8_LCD_IMAGE_ROW_SIZE_50PX][U8_LCD_IMAGE_COL_BYTES_50PX];
extern const u8 DefaultBackKeys[U8_LCD_IMAGE_ROW_SIZE_50PX][U8_LCD_IMAGE_COL_BYTES_50PX];
extern const u8 CKey[U8_LCD_IMAGE_ROW_SIZE_50PX][U8_LCD_IMAGE_COL_BYTES_50PX];
extern const u8 DKey[U8_LCD_IMAGE_ROW_SIZE_50PX][U8_LCD_IMAGE_COL_BYTES_50PX];
extern const u8 EKey[U8_LCD_IMAGE_ROW_SIZE_50PX][U8_LCD_IMAGE_COL_BYTES_50PX];
extern const u8 FKeyFront[U8_LCD_IMAGE_ROW_SIZE_50PX][U8_LCD_IMAGE_COL_BYTES_50PX];
extern const u8 CSharpKey[U8_LCD_IMAGE_ROW_SIZE_50PX][U8_LCD_IMAGE_COL_BYTES_50PX];
extern const u8 DSharpKey[U8_LCD_IMAGE_ROW_SIZE_50PX][U8_LCD_IMAGE_COL_BYTES_50PX];
extern const u8 DefaultFrontKeys[U8_LCD_IMAGE_ROW_SIZE_50PX][U8_LCD_IMAGE_COL_BYTES_50PX];
//Locations of the keys and text
static PixelBlockType front;
static PixelBlockType back;
const static PixelAddressType textLocation = {0, 18};
static PixelBlockType clear;
//Global Variables for specific purposes
static u8 u8NoteIndex = 0;
static u8 notesPlayed = 0;
static u8 newNotes = 0;
static u8 count = 0;
static u8 octave = 1;
static u8 differentLetter = 0;
static u8 currentNumberOfNotes = 0;
static char sameNote;
//Array for text, notes and color
static char text[4][20] = {"Octave: Third", "Octave: Fourth",
                          "Octave: Fifth", "Octave: Sixth"};
const static u8 aau8Color[7][3] = {{RED0, 0xff, 0xff}, 
                                 {RED0, GREEN0, 0xff}, 
                                 {0xff, GREEN0, 0xff},
                                 {0xff, GREEN0, BLUE0},
                                 {0xff, 0xff, BLUE0},
                                 {RED0, 0xff, BLUE0},
                                 {RED0, GREEN0, BLUE0},};
static u16 au16Notes[4][12] = {{C3, C3S, D3, D3S, E3, F3, F3S, G3, G3S, A3, A3S, B3},
                              {C4, C4S, D4, D4S, E4, F4, F4S, G4, G4S, A4, A4S, B4},
                              {C5, C5S, D5, D5S, E5, F5, F5S, G5, G5S, A5, A5S, B5},
                              {C6, C6S, D6, D6S, E6, F6, F6S, G6, G6S, A6, A6S, B6}};
        
/**********************************************************************************************************************
Function Definitions
**********************************************************************************************************************/
void leds();
void buzzer();
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
  // Loads Welcome Screen and texts
  const u8 Welcome[] = {"Piano Master"};
  const static PixelAddressType startLocation = {10, 30};
  LcdClearScreen();
  LcdLoadString(Welcome, LCD_FONT_SMALL, &startLocation);
  PixelBlockType startImage;
  startImage.u16RowStart = 25;
  startImage.u16ColumnStart = 53;
  startImage.u16RowSize = 25;
  startImage.u16ColumnSize = 25;
  LcdLoadBitmap(&startLogo[0][0], &startImage);
  // Initializes specific functions and values
  PWMAudioSetFrequency(BUZZER1, 500);
  front.u16RowStart = 10;
  front.u16ColumnStart = 15;
  front.u16RowSize = 50;
  front.u16ColumnSize = 50;
  back.u16RowStart = 10;
  back.u16ColumnStart = 65;
  back.u16RowSize = 50;
  back.u16ColumnSize = 50;
  clear.u16ColumnSize = 100;
  clear.u16RowSize = 7;
  clear.u16ColumnStart = 18;
  clear.u16RowStart = 0;
  DebugSetPassthrough();
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
  //Keeps track of the number of notes played in Tera Term
  notesPlayed = G_u8DebugScanfCharCount;
  //Clears the Ram and newNotes in order to keep playing
  if(G_u8DebugScanfCharCount >= DEBUG_SCANF_BUFFER_SIZE)
  {
    for(int i = 0; i < DEBUG_SCANF_BUFFER_SIZE; i++)
    {
      G_au8DebugScanfBuffer[i]='\0';
    }
    newNotes = 0;
    G_u8DebugScanfCharCount = 0;
    notesPlayed = 0;
    u8NoteIndex = 0;
    newNotes = 0;
    count = 0;
    differentLetter = 0;
    currentNumberOfNotes = 0;
    PWMAudioOff(BUZZER1);
  }
  //Conditional Statement that reset the time a note is played if multiple notes are pressed
  if(currentNumberOfNotes < G_u8DebugScanfCharCount)
  {
    count = 0;
    newNotes = notesPlayed - 1;
  }
  //Conditional Statement that plays when a new letter is entered into the keyboard
  if(differentLetter < G_u8DebugScanfCharCount)
  {
    buzzer();
    differentLetter++;
    currentNumberOfNotes = G_u8DebugScanfCharCount;
  }
  //Conditional Statements that change the octave when the buttons are pressed
  if (WasButtonPressed(BUTTON0) && octave > 0)
  {
    ButtonAcknowledge(BUTTON0);
    octave--;
    LcdClearPixels(&clear);
    LcdLoadString(text[octave], LCD_FONT_SMALL, &textLocation);
  }
  else if (WasButtonPressed(BUTTON1) && octave < 3)
  {
    ButtonAcknowledge(BUTTON1);
    octave++;
    LcdClearPixels(&clear);
    LcdLoadString(text[octave], LCD_FONT_SMALL, &textLocation);
  }
  // Sets the Audio Frequency
  PWMAudioSetFrequency(BUZZER1, au16Notes[octave][u8NoteIndex]);
  // Loads the outputs of screen buzzer and lights
  if (notesPlayed > newNotes)
  {
    PWMAudioOn(BUZZER1);
    LcdClearScreen();
    LcdLoadBitmap(assignedFront, &front);
    LcdLoadBitmap(assignedBack, &back);
    LcdLoadString(text[octave], LCD_FONT_SMALL, &textLocation);
    led();
    count++;
    // Plays each note for 0.5s
    if(count >= 100)
    {
      newNotes++;
      count = 0;
      PWMAudioOff(BUZZER1);
      // Goes back to default screen
      if (sameNote != G_au8DebugScanfBuffer[G_u8DebugScanfCharCount - 1])
      {
        LcdLoadBitmap(&DefaultFrontKeys[0][0], &front);
        LcdLoadBitmap(&DefaultBackKeys[0][0], &back);
      }
      sameNote = G_au8DebugScanfBuffer[G_u8DebugScanfCharCount - 1];
    } 
  }
} /* end UserApp1SM_Idle() */

/**
 * Turns the Leds a certain color based on keyboard input
 * @param None
 * @return None
 */
void led()
{
  static u8 color = NULL;
  // Resets all Leds to off
  for(u8 ledNumber = 0; ledNumber < 4; ledNumber++)
    {
      for (u8 ledColor = 0; ledColor < 3; ledColor++)
        LedOff((aau8Color[6][ledColor])+ ledNumber);
    }
  // Assigns new Led colors based on input
  switch (G_au8DebugScanfBuffer[G_u8DebugScanfCharCount - 1])
    {
      case 'z':
        color = 0;
        break;
      case 's':
        color = 0;
        break;
      case 'x':
        color = 1;
        break;
      case 'd':
        color = 1;
        break;
      case 'c':
        color = 2;
        break;
      case 'v':
        color = 3;
        break;
      case 'g':
        color = 3;
        break;
      case 'b':
        color = 4;
        break;
      case 'h':
        color = 4;
        break;
      case 'n':
        color = 5;
        break;
      case 'j':
        color = 5;
        break;
      case 'm':
        color = 6;
        break;
    }
    // Activates the specific colors
    for(u8 i = 0; i < 3; i++)
    {
      if(aau8Color[color][i] != 0xff)
      {
        for (u8 j = 0; j < 4; j++)
          LedOn((aau8Color[color][i])+ j);
      }
    }
}

/**
 * Turns the buzzer on or off based on Input
 * @param None
 * @return None
 */
void buzzer()
{
  //Assigns all buzzer notes
  switch (G_au8DebugScanfBuffer[G_u8DebugScanfCharCount - 1])
    {
      case 'z':
        u8NoteIndex = 0;
        assignedFront = &CKey[0][0];
        assignedBack = &DefaultBackKeys[0][0];
        break;
      case 's':
        u8NoteIndex = 1;
        assignedFront = &CSharpKey[0][0];
        assignedBack = &DefaultBackKeys[0][0];
        break;
      case 'x':
        u8NoteIndex = 2;
        assignedFront = &DKey[0][0];
        assignedBack = &DefaultBackKeys[0][0];
        break;
      case 'd':
        u8NoteIndex = 3;
        assignedFront = &DSharpKey[0][0];
        assignedBack = &DefaultBackKeys[0][0];
        break;
      case 'c':
        u8NoteIndex = 4;
        assignedFront = &EKey[0][0];
        assignedBack = &DefaultBackKeys[0][0];
        break;
      case 'v':
        u8NoteIndex = 5;
        assignedFront = &FKeyFront[0][0];
        assignedBack = &FKeyBack[0][0];
        break;
      case 'g':
        u8NoteIndex = 6;
        assignedFront = &DefaultFrontKeys[0][0];
        assignedBack = &FSharpKey[0][0];
        break;
      case 'b':
        u8NoteIndex = 7;
        assignedFront = &DefaultFrontKeys[0][0];
        assignedBack = &GKey[0][0];
        break;
      case 'h':
        u8NoteIndex = 8;
        assignedFront = &DefaultFrontKeys[0][0];
        assignedBack = &GSharpKey[0][0];
        break;
      case 'n':
        u8NoteIndex = 9;
        assignedFront = &DefaultFrontKeys[0][0];
        assignedBack = &AKey[0][0];
        break;
      case 'j':
        u8NoteIndex = 10;
        assignedFront = &DefaultFrontKeys[0][0];
        assignedBack = &ASharpKey[0][0];
        break;
      case 'm':
        u8NoteIndex = 11;
        assignedFront = &DefaultFrontKeys[0][0];
        assignedBack = &BKey[0][0];
        break;
      // Holds note
      case ' ':
        newNotes++;
        assignedFront = &DefaultFrontKeys[0][0];
        assignedBack = &DefaultBackKeys[0][0];
        break;
      default:
        newNotes++;
        assignedFront = &DefaultFrontKeys[0][0];
        assignedBack = &DefaultBackKeys[0][0];
        PWMAudioOff(BUZZER1);
        break;
    }
}

/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp1SM_Error(void)          
{
  
} /* end UserApp1SM_Error() */

/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
