

#include <asf.h>
#include <stdio_serial.h>
#include <string.h>
#include "conf_test.h"

#define TEST_EEPROM_PAGE    1
#define TEST_BUFFER_SIZE    19
#define TEST_BUFFER_OFFSET  123

/* Structure for UART module connected to EDBG (used for unit test output) */
struct usart_module cdc_uart_module;
/* Structure to store EEPROM emulator parameters */
struct eeprom_emulator_parameters ee_params;

/* Data buffers used in test */
uint8_t test_buffer[TEST_BUFFER_SIZE];
uint8_t verify_buffer[TEST_BUFFER_SIZE];
uint8_t test_page[EEPROM_PAGE_SIZE];
uint8_t verify_page[EEPROM_PAGE_SIZE];
uint16_t i;

/* Flag used to check successful initialization */
volatile bool init_success;

/**
 * \brief Initialize the USART for unit test
 *
 * Initializes the SERCOM USART used for sending the unit test status to the
 * computer via the EDBG CDC gateway.
 */
static void cdc_uart_init(void)
{
	struct usart_config usart_conf;

	/* Configure USART for unit test output */
	usart_get_config_defaults(&usart_conf);
	usart_conf.mux_setting = CONF_STDIO_MUX_SETTING;
	usart_conf.pinmux_pad0 = CONF_STDIO_PINMUX_PAD0;
	usart_conf.pinmux_pad1 = CONF_STDIO_PINMUX_PAD1;
	usart_conf.pinmux_pad2 = CONF_STDIO_PINMUX_PAD2;
	usart_conf.pinmux_pad3 = CONF_STDIO_PINMUX_PAD3;
	usart_conf.baudrate    = CONF_STDIO_BAUDRATE;

	stdio_serial_init(&cdc_uart_module, CONF_STDIO_USART, &usart_conf);
	usart_enable(&cdc_uart_module);
}


static void run_eeprom_init_test(const struct test_case *test)
{
	enum status_code status;
	init_success = true;

	/* Initialize the EEPROM emulator */
	status = eeprom_emulator_init();
	if (status == STATUS_ERR_NO_MEMORY) {
		init_success = false;
	}

	test_assert_true(test, init_success,
			"No EEPROM memory section has been set in the device's fuses");

	/* Format memory if needed */
	if (status != STATUS_OK) {
		eeprom_emulator_erase_memory();
		status = eeprom_emulator_init();
		if (status != STATUS_OK) {
			init_success = false;
		}
	}

	test_assert_true(test, status == STATUS_OK,
			"EEPROM emulator initialization failed");

	/* Get parameter check */
	status = eeprom_emulator_get_parameters(&ee_params);
	if (status != STATUS_OK) {
		init_success = false;
	}

	test_assert_true(test, status == STATUS_OK,
			"EEPROM parameter retrieve failed");
	test_assert_true(test, ee_params.page_size == EEPROM_PAGE_SIZE,
			"Get parameter returned wrong page size"
			"Expected %d - Returned %d", EEPROM_PAGE_SIZE,
			ee_params.page_size);
}

/**
 * \internal
 * \brief Setup function for EEPROM emulator buffer read/write test.
 *
 * This function initializes the test buffer and verify buffer with
 * appropriate values.
 *
 * \param test Current test case.
 */
static void setup_eeprom_buffer_read_write_test(const struct test_case *test)
{
	/* Initialize buffers */
	for (i = 0; i < TEST_BUFFER_SIZE; i++) {
		test_buffer[i] = i + 1;
		verify_buffer[i] = 0;
	}
}

/**
 * \internal
 * \brief Test for EEPROM emulator buffer read/write function.
 *
 * This test writes a buffer of data of a given length to a given
 * offset. It then reads back the data from that offset and verifies.
 *
 * \param test Current test case.
 */
static void run_eeprom_buffer_read_write_test(const struct test_case *test)
{
	volatile enum status_code status;

	/* Skip test if initialization failed */
	test_assert_true(test, init_success,
			"Skipping test due to failed initialization");

	/* Write buffer test */
	status = eeprom_emulator_write_buffer(TEST_BUFFER_OFFSET,
			test_buffer, TEST_BUFFER_SIZE);
	test_assert_true(test, status == STATUS_OK,
			"EEPROM write buffer failed");

	/* Write back to physical NVM memory */
	eeprom_emulator_commit_page_buffer();

	/* Read buffer test */
	status = eeprom_emulator_read_buffer(TEST_BUFFER_OFFSET,
			verify_buffer, TEST_BUFFER_SIZE);
	test_assert_true(test, status == STATUS_OK,
			"EEPROM read buffer failed");

	/* Verify buffer test */
	for (i = 0; i < TEST_BUFFER_SIZE; i++) {
		test_assert_true(test, test_buffer[i] == verify_buffer[i],
				"Buffer data incorrect at index %d: "
				"Expected %d - Read %d", i, test_buffer[i],
				verify_buffer[i]);
	}
}

/**
 * \internal
 * \brief Setup function for EEPROM emulator page read/write test.
 *
 * This function initializes the test_page and verify_page data arrays
 * with appropriate values.
 *
 * \param test Current test case.
 */
static void setup_eeprom_page_read_write_test(const struct test_case *test)
{
	/* Initialize data arrays */
	for (i = 0; i < EEPROM_PAGE_SIZE; i++) {
		test_page[i] = i + 1;
		verify_page[i] = 0;
	}
}

/**
 * \internal
 * \brief Test for EEPROM emulator page read/write function.
 *
 * This test writes a page of data to a given page number.
 * It then reads back the page and verifies.
 *
 * \param test Current test case.
 */
static void run_eeprom_page_read_write_test(const struct test_case *test)
{
	volatile enum status_code status;

	/* Skip test if initialization failed */
	test_assert_true(test, init_success,
			"Skipping test due to failed initialization");

	/* Write page test */
	status = eeprom_emulator_write_page(TEST_EEPROM_PAGE, test_page);
	test_assert_true(test, status == STATUS_OK,
			"EEPROM write page failed");

	/* Write back to physical NVM memory */
	eeprom_emulator_commit_page_buffer();

	/* Read page test */
	status = eeprom_emulator_read_page(TEST_EEPROM_PAGE, verify_page);
	test_assert_true(test, status == STATUS_OK,
			"EEPROM read page failed");

	/* Verify page test */
	for (i = 0; i < EEPROM_PAGE_SIZE; i++) {
		test_assert_true(test, test_page[i] == verify_page[i],
				"Page data incorrect at index %d: "
				"Expected %d - Read %d", i, test_page[i],
				verify_page[i]);
	}
}

/**
 * \brief Run EEPROM emulator unit tests
 *
 * Initializes the system and serial output, then sets up the
 * EEPROM emulator unit test suite and runs it.
 */
int main(void)
{
	system_init();
	cdc_uart_init();

	/* Define Test Cases */
	DEFINE_TEST_CASE(eeprom_init_test, NULL,
			run_eeprom_init_test, NULL,
			"Testing EEPROM emulator initialization");

	DEFINE_TEST_CASE(eeprom_buffer_read_write_test,
			setup_eeprom_buffer_read_write_test,
			run_eeprom_buffer_read_write_test, NULL,
			"Testing EEPROM buffer read/write functionality");

	DEFINE_TEST_CASE(eeprom_page_read_write_test,
			setup_eeprom_page_read_write_test,
			run_eeprom_page_read_write_test, NULL,
			"Testing EEPROM page read/write functionality");

	/* Put test case addresses in an array */
	DEFINE_TEST_ARRAY(eeprom_tests) = {
		&eeprom_init_test,
		&eeprom_buffer_read_write_test,
		&eeprom_page_read_write_test,
	};

	/* Define the test suite */
	DEFINE_TEST_SUITE(eeprom_test_suite, eeprom_tests,
			"SAM EEPROM emulator service test suite");

	/* Run all tests in the suite*/
	test_suite_run(&eeprom_test_suite);

	while (true) {
		/* Intentionally left empty */
	}
}
