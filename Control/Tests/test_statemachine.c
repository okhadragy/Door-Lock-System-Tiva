/*****************************************************
 * FULL STATE MACHINE TRANSITION TEST SUITE
 * Test File: test_statemachine.c
 * Author: Mohamed Morsi
 *
 * IMPORTANT:
 * - PuTTY SIMULATES THE OTHER TIVA (HMI ECU) when you type
 *   characters or watch UART output.
 * - Every test case below is a function.
 * - You MUST trigger each test manually by calling it
 *   in main() one at a time (to avoid sequence mixing).
 * - This file uses direct BUFFER injection via inject_char().
 *****************************************************/

#include "../logic/statemachine.h"
#include "../logic/password.h"
#include "../logic/buffer.h"
#include "../Drivers/uart.h"
#include "../Drivers/eeprom.h"
#include "../Drivers/timer.h"
#include "../Drivers/motor.h"
#include "../Drivers/buzzer.h"

extern SystemState currentState;
extern SystemActions currentAction;
extern int failCount;
extern char BUFFER[];
extern unsigned int bufferIndex;

/* Helper: inject one char as if UART ISR did it */
static void inject_char(char c) {
    BUFFER[bufferIndex++] = c;
}

/* Helper: inject string (does not auto-call StateMachine) */
static void inject_string(const char *s) {
    while (*s) inject_char(*s++);
}

/* Helper: clear buffer (assumes ResetBuffer exists as in your project) */
static void reset_input_buffer(void) {
    ResetBuffer(); /* your ResetBuffer should set bufferIndex=0 and zero BUFFER */
}

/* Helper: write a known password into EEPROM for tests that need it
   This uses the same API your FSM calls to store password.
   We choose "12345" as a test password. */
static void ensure_password_in_eeprom(void) {
    char pw[PASSWORD_LENGTH + 1] = "12345";
    EEPROM_WritePassword(pw);
}

/*************************************************************
 * HOW TO VERIFY TEST CASES WITH PUTTY
 *
 * - For tests with EXPECTED OUTPUT:
 *      Compare the actual UART output to the "Expected Output:" comment.
 *
 * - For tests with NO EXPECTED OUTPUT:
 *      Verify by:
 *         • No extra characters appear in PuTTY when partial input is sent
 *         • The system accepts the next input and produces the expected
 *           output when the input becomes complete
 *         • Or observe the physical output (motor movement / buzzer)
 *
 * - For tests that rely on EEPROM-stored password:
 *      The test writes the known password to EEPROM before exercising the FSM,
 *      so you can run each test individually.
 *************************************************************/


/*************************************************************
 * TEST 1 — INIT → MAIN_MENU (Initial Password Setup)
 *
 * Purpose:
 *      Verify that a valid 5-digit password in STATE_INIT
 *      moves the system into STATE_MAIN_MENU and writes EEPROM.
 *
 * Preconditions:
 *      System may be in any state; we set STATE_INIT.
 *
 * Putty Input:
 *      1 2 3 4 5
 *
 * Expected Output:
 *      "S"
 *
 * Expected Transition:
 *      STATE_INIT -> STATE_MAIN_MENU
 *************************************************************/
void test_init_to_main_menu(void) {
    UART0_Send_String("\n--- RUN TEST 1: INIT -> MAIN_MENU ---\n");
    currentState = STATE_INIT;
    reset_input_buffer();

    inject_char('1');
    inject_char('2');
    inject_char('3');
    inject_char('4');
    inject_char('5');

    StateMachine(); /* EXPECT "S" and password saved to EEPROM */
}

/*************************************************************
 * TEST 2 — MAIN_MENU Invalid Choice (single-char)
 *
 * Purpose:
 *      Ensure invalid menu inputs print "!" and system stays in MAIN_MENU.
 *
 * Preconditions:
 *      currentState = STATE_MAIN_MENU
 *
 * Putty Input:
 *      X
 *
 * Expected Output:
 *      "!"
 *
 * Expected Transition:
 *      STATE_MAIN_MENU -> STATE_MAIN_MENU
 *************************************************************/
void test_mainmenu_invalid(void) {
    UART0_Send_String("\n--- RUN TEST 2: MAIN_MENU INVALID ---\n");
    currentState = STATE_MAIN_MENU;
    reset_input_buffer();

    inject_char('X'); /* invalid */
    StateMachine(); /* EXPECT "!" */
}

/*************************************************************
 * TEST 3 — MAIN_MENU '+' -> CHECK_PASSWORD (ACTION_OPEN_DOOR)
 *
 * Purpose:
 *      Selecting '+' should set action and transition to CHECK_PASSWORD.
 *
 * Preconditions:
 *      currentState = STATE_MAIN_MENU
 *
 * Putty Input:
 *      +
 *
 * Expected Output:
 *      "S"
 *
 * Expected Transition:
 *      STATE_MAIN_MENU -> STATE_CHECK_PASSWORD (currentAction = ACTION_OPEN_DOOR)
 *************************************************************/
void test_mainmenu_plus(void) {
    UART0_Send_String("\n--- RUN TEST 3: MAIN_MENU '+' ---\n");
    currentState = STATE_MAIN_MENU;
    reset_input_buffer();

    inject_char('+');
    StateMachine(); /* EXPECT "S" */
}

/*************************************************************
 * TEST 4 — MAIN_MENU '-' -> CHECK_PASSWORD (ACTION_CHANGE_PASSWORD)
 *
 * Purpose:
 *      Selecting '-' should set action and transition to CHECK_PASSWORD.
 *
 * Input: -
 * Output: "S"
 * Transition: MAIN_MENU -> CHECK_PASSWORD (currentAction = ACTION_CHANGE_PASSWORD)
 *************************************************************/
void test_mainmenu_minus(void) {
    UART0_Send_String("\n--- RUN TEST 4: MAIN_MENU '-' ---\n");
    currentState = STATE_MAIN_MENU;
    reset_input_buffer();

    inject_char('-');
    StateMachine(); /* EXPECT "S" */
}

/*************************************************************
 * TEST 5 — MAIN_MENU '*' -> CHECK_PASSWORD (ACTION_SET_TIMEOUT)
 *
 * Purpose:
 *      Selecting '*' should set action and transition to CHECK_PASSWORD.
 *
 * Input: *
 * Output: "S"
 * Transition: MAIN_MENU -> CHECK_PASSWORD (currentAction = ACTION_SET_TIMEOUT)
 *************************************************************/
void test_mainmenu_star(void) {
    UART0_Send_String("\n--- RUN TEST 5: MAIN_MENU '*' ---\n");
    currentState = STATE_MAIN_MENU;
    reset_input_buffer();

    inject_char('*');
    StateMachine(); /* EXPECT "S" */
}

/*************************************************************
 * TEST 6 — CHECK_PASSWORD Correct (Open Door)
 *
 * Purpose:
 *      With currentAction = ACTION_OPEN_DOOR, entering correct 5-digit
 *      password should open door, wait autolock delay (from EEPROM),
 *      then close door and return to MAIN_MENU.
 *
 * Preconditions:
 *      EEPROM contains known password (test writes it).
 *      currentAction = ACTION_OPEN_DOOR
 *
 * Putty Input:
 *      1 2 3 4 5
 *
 * Expected Output:
 *      "S" (password OK) then (after open_Door) "S"
 *      Motor rotates to open and later to close (observe physically)
 *
 * Expected Transition:
 *      STATE_CHECK_PASSWORD -> STATE_MAIN_MENU
 *************************************************************/
void test_checkpassword_open_success(void) {
    UART0_Send_String("\n--- RUN TEST 6: CHECK_PASSWORD OPEN SUCCESS ---\n");
    ensure_password_in_eeprom();
    currentState = STATE_CHECK_PASSWORD;
    currentAction = ACTION_OPEN_DOOR;
    reset_input_buffer();

    inject_char('1');
    inject_char('2');
    inject_char('3');
    inject_char('4');
    inject_char('5');

    StateMachine(); /* EXPECT "S", open_Door action, delay, close_Door, "S" */
}

/*************************************************************
 * TEST 7 — CHECK_PASSWORD Correct -> CHANGE_PASSWORD state
 *
 * Purpose:
 *      With currentAction = ACTION_CHANGE_PASSWORD, correct password
 *      should move to STATE_CHANGE_PASSWORD (so next user entry will
 *      be the new password saved).
 *
 * Preconditions:
 *      EEPROM contains known password
 *
 * Input:
 *      1 2 3 4 5
 *
 * Expected Output:
 *      "S"
 * Expected Transition:
 *      STATE_CHECK_PASSWORD -> STATE_CHANGE_PASSWORD
 *************************************************************/
void test_checkpassword_change_password_success(void) {
    UART0_Send_String("\n--- RUN TEST 7: CHECK_PASSWORD -> CHANGE_PASSWORD ---\n");
    ensure_password_in_eeprom();
    currentState = STATE_CHECK_PASSWORD;
    currentAction = ACTION_CHANGE_PASSWORD;
    reset_input_buffer();

    inject_char('1');
    inject_char('2');
    inject_char('3');
    inject_char('4');
    inject_char('5');

    StateMachine(); /* EXPECT "S" and currentState == STATE_CHANGE_PASSWORD */
}

/*************************************************************
 * TEST 8 — CHECK_PASSWORD Correct -> SET_TIMEOUT state
 *
 * Purpose:
 *      With currentAction = ACTION_SET_TIMEOUT, correct password
 *      should move to STATE_SET_TIMEOUT.
 *
 * Preconditions:
 *      EEPROM contains known password
 *
 * Input: 1 2 3 4 5
 * Output: "S"
 * Transition: STATE_CHECK_PASSWORD -> STATE_SET_TIMEOUT
 *************************************************************/
void test_checkpassword_settimeout_success(void) {
    UART0_Send_String("\n--- RUN TEST 8: CHECK_PASSWORD -> SET_TIMEOUT ---\n");
    ensure_password_in_eeprom();
    currentState = STATE_CHECK_PASSWORD;
    currentAction = ACTION_SET_TIMEOUT;
    reset_input_buffer();

    inject_char('1');
    inject_char('2');
    inject_char('3');
    inject_char('4');
    inject_char('5');

    StateMachine(); /* EXPECT "S" and currentState == STATE_SET_TIMEOUT */
}

/*************************************************************
 * TEST 9 — CHECK_PASSWORD Wrong Password (single fail)
 *
 * Purpose:
 *      Wrong password should print "F" and increment failCount.
 *
 * Preconditions:
 *      currentAction = ACTION_OPEN_DOOR
 *
 * Input: 9 9 9 9 9
 * Output: "F"
 * Expected: failCount incremented by 1
 *************************************************************/
void test_checkpassword_fail_once(void) {
    UART0_Send_String("\n--- RUN TEST 9: CHECKPASSWORD FAIL 1 ---\n");
    currentState = STATE_CHECK_PASSWORD;
    currentAction = ACTION_OPEN_DOOR;
    failCount = 0;
    reset_input_buffer();

    inject_char('9');
    inject_char('9');
    inject_char('9');
    inject_char('9');
    inject_char('9');

    StateMachine(); /* EXPECT "F" and failCount == 1 */
}

/*************************************************************
 * TEST 10 — CHECK_PASSWORD Fail 3 times -> Buzzer Lockout
 *
 * Purpose:
 *      Three consecutive wrong passwords trigger buzzer for 5s,
 *      then print "S" and reset failCount.
 *
 * Steps:
 *    Enter wrong password 3 times sequentially
 *
 * Expected Output:
 *    "F" "F" "F" then 5s buzzer then "S"
 * Expected Transition:
 *    After buzzer, STATE_MAIN_MENU and failCount == 0
 *************************************************************/
void test_checkpassword_fail_three(void) {
    UART0_Send_String("\n--- RUN TEST 10: CHECKPASSWORD FAIL 3 TIMES ---\n");
    currentState = STATE_CHECK_PASSWORD;
    currentAction = ACTION_OPEN_DOOR;
    failCount = 0;

    for (int i = 0; i < 3; ++i) {
        reset_input_buffer();
        inject_char('9');
        inject_char('9');
        inject_char('9');
        inject_char('9');
        inject_char('9');
        StateMachine(); /* Expect "F" for each of first 3, then buzzer & "S" */
    }
}

/*************************************************************
 * TEST 11 — CHANGE_PASSWORD full flow
 *
 * Purpose:
 *   1) From MAIN_MENU select '-' to start change-password
 *   2) Enter correct old password
 *   3) Enter new 5-digit password
 *
 * Steps and Expected Outputs:
 *   Step1: inject '-' -> "S" and move to CHECK_PASSWORD
 *   Step2: inject old password -> "S" and move to CHANGE_PASSWORD
 *   Step3: inject new password (5 digits) -> "S" and return to MAIN_MENU
 *************************************************************/
void test_change_password_flow(void) {
    UART0_Send_String("\n--- RUN TEST 11: CHANGE_PASSWORD FLOW ---\n");
    ensure_password_in_eeprom();

    /* Step 1: MAIN_MENU -> select '-' */
    currentState = STATE_MAIN_MENU;
    reset_input_buffer();
    inject_char('-');
    StateMachine(); /* EXPECT "S" */

    /* Step 2: CHECK_PASSWORD -> enter correct old password */
    currentState = STATE_CHECK_PASSWORD;
    currentAction = ACTION_CHANGE_PASSWORD;
    reset_input_buffer();
    inject_string("12345");
    StateMachine(); /* EXPECT "S" and currentState == STATE_CHANGE_PASSWORD */

    /* Step 3: CHANGE_PASSWORD -> enter new password (55555) */
    currentState = STATE_CHANGE_PASSWORD;
    reset_input_buffer();
    inject_string("55555");
    StateMachine(); /* EXPECT "S" and EEPROM updated with new password */
}

/*************************************************************
 * TEST 12 — SET_TIMEOUT Valid (2 digits)
 *
 * Purpose:
 *      With STATE_SET_TIMEOUT and a 2-digit input (TIMEOUT_LENGTH=2),
 *      the FSM must accept the input, convert it, write to EEPROM,
 *      print "S" and return to MAIN_MENU.
 *
 * Preconditions:
 *      currentState = STATE_SET_TIMEOUT
 *
 * Input: 1 0  (example valid value 10)
 * Output: "S"
 *************************************************************/
void test_settimeout_valid_10(void) {
    UART0_Send_String("\n--- RUN TEST 12: SET_TIMEOUT VALID '05' ---\n");
    currentState = STATE_SET_TIMEOUT;
    reset_input_buffer();

    inject_char('1');
    inject_char('0'); /* 10 */

    StateMachine(); /* EXPECT "S" and EEPROM write */
}

/*************************************************************
 * TEST 13 — FULL FLOW: Set Timeout via menu, then open door uses new timeout
 *
 * Purpose:
 *   1) From MAIN_MENU select '*' -> CHECK_PASSWORD
 *   2) Enter correct password -> move to SET_TIMEOUT
 *   3) Enter 2-digit timeout '10' -> saved
 *   4) From MAIN_MENU select '+' -> CHECK_PASSWORD
 *   5) Enter correct password -> open door and wait for saved timeout (observe)
 *
 * Preconditions:
 *   Known password in EEPROM
 *
 * Expected:
 *   All intermediate prints as per FSM, motor waits for new timeout value.
 *************************************************************/
void test_full_settimeout_then_open(void) {
    UART0_Send_String("\n--- RUN TEST 15: SET_TIMEOUT FLOW THEN OPEN DOOR ---\n");
    ensure_password_in_eeprom();

    /* Step 1: MAIN_MENU '*' */
    currentState = STATE_MAIN_MENU;
    reset_input_buffer();
    inject_char('*');
    StateMachine(); /* EXPECT "S" -> CHECK_PASSWORD */

    /* Step 2: enter password to authorize SET_TIMEOUT */
    currentState = STATE_CHECK_PASSWORD;
    currentAction = ACTION_SET_TIMEOUT;
    reset_input_buffer();
    inject_string("12345");
    StateMachine(); /* EXPECT "S" and move to STATE_SET_TIMEOUT */

    /* Step 3: enter timeout 10 (2 digits) */
    currentState = STATE_SET_TIMEOUT;
    reset_input_buffer();
    inject_string("10");
    StateMachine(); /* EXPECT "S" and EEPROM write */

    /* Step 4: MAIN_MENU '+', then password to open door */
    currentState = STATE_MAIN_MENU;
    reset_input_buffer();
    inject_char('+');
    StateMachine(); /* EXPECT "S" -> CHECK_PASSWORD */

    currentState = STATE_CHECK_PASSWORD;
    currentAction = ACTION_OPEN_DOOR;
    reset_input_buffer();
    inject_string("12345");
    StateMachine(); /* EXPECT open_Door, wait ~10s (the saved timeout), then close_Door */
}

/*************************************************************
 * Test runner note:
 * - Tests are designed so you can run them individually.
 * - If you want to run a sequence, ensure the EEPROM contains
 *   the required password before tests that depend on it.
 *************************************************************/