#include "TestFramework.h"

#include "GUtils.h"

// STD
#include <stdlib.h>
#include <string.h>

/* == Txt == */

TEST(txt_returns_the_same_string)
{
  const char *color = RED;

  CHECK_TRUE( Txt(color) == color );
  CHECK_EQ_STR( Txt("plain"), "plain" );
}

/* == PrtError / PrtDbgError == */

TEST(prt_error_prints_the_message_and_the_code)
{
  char out[512];

  CaptureStdoutStart();
  PrtError("End of file reached.", 254000);
  CaptureStdoutStop(out, sizeof(out));

  CHECK_EQ_STR( out, RED "End of file reached. ERROR CODE: " MAGENTA "254000" CRESET "\n" );
}

TEST(prt_error_prints_negative_codes)
{
  char out[512];

  CaptureStdoutStart();
  PrtError("Unknown.", -1);
  CaptureStdoutStop(out, sizeof(out));

  CHECK_NOT_NULL( strstr(out, "ERROR CODE: ") );
  CHECK_NOT_NULL( strstr(out, "-1") );
}

TEST(prt_dbg_error_prints_the_message_and_the_origin)
{
  char out[512];

  CaptureStdoutStart();
  PrtDbgError("Unexpected Error.", "LI -> GetInput");
  CaptureStdoutStop(out, sizeof(out));

  CHECK_EQ_STR( out, RED "Unexpected Error. " YELLOW "LI -> GetInput" CRESET );
}

/* == PtrVerify == */

TEST(ptr_verify_returns_a_valid_pointer_untouched)
{
  char out[512];
  int value = 0;

  CaptureStdoutStart();
  void *ret = PtrVerify(&value, "Allocation Error.", "test");
  const size_t written = CaptureStdoutStop(out, sizeof(out));

  CHECK_TRUE( ret == &value );
  CHECK_EQ_SIZE( written, 0 ); // Nothing is reported on success
}

TEST(ptr_verify_reports_null_and_returns_null)
{
  char out[512];

  CaptureStdoutStart();
  void *ret = PtrVerify(NULL, "Allocation Error.", "test origin");
  CaptureStdoutStop(out, sizeof(out));

  CHECK_NULL( ret );
  CHECK_NOT_NULL( strstr(out, "Allocation Error.") );
  CHECK_NOT_NULL( strstr(out, "test origin") );
}

/* == AllocPtr / ReallocPtr == */

TEST(alloc_ptr_returns_usable_memory)
{
  char *buffer = (char *)AllocPtr(16 * sizeof(char));

  CHECK_NOT_NULL( buffer );
  if ( buffer == NULL )
    return;

  memcpy(buffer, "droga", sizeof("droga"));
  CHECK_EQ_STR( buffer, "droga" );

  free(buffer);
}

TEST(realloc_ptr_keeps_the_previous_content)
{
  char *buffer = (char *)AllocPtr(8 * sizeof(char));
  memcpy(buffer, "droga", sizeof("droga"));

  buffer = (char *)ReallocPtr(64 * sizeof(char), buffer);

  CHECK_NOT_NULL( buffer );
  if ( buffer == NULL )
    return;

  CHECK_EQ_STR( buffer, "droga" );

  free(buffer);
}

TEST(realloc_ptr_from_null_behaves_as_an_allocation)
{
  char *buffer = (char *)ReallocPtr(16 * sizeof(char), NULL);

  CHECK_NOT_NULL( buffer );
  free(buffer);
}

/* == AllocPPtr / ReallocPPtr == */

TEST(alloc_pptr_returns_an_usable_pointer_array)
{
  char first[] = "uno";
  char second[] = "dos";
  char **array = (char **)AllocPPtr(2 * sizeof(char *));

  CHECK_NOT_NULL( array );
  if ( array == NULL )
    return;

  array[0] = first;
  array[1] = second;
  CHECK_EQ_STR( array[0], "uno" );
  CHECK_EQ_STR( array[1], "dos" );

  free(array);
}

TEST(realloc_pptr_keeps_the_previous_pointers)
{
  char first[] = "uno";
  char **array = (char **)AllocPPtr(1 * sizeof(char *));
  array[0] = first;

  array = (char **)ReallocPPtr(4 * sizeof(char *), (void **)array);

  CHECK_NOT_NULL( array );
  if ( array == NULL )
    return;

  CHECK_TRUE( array[0] == first );

  free(array);
}

TEST(realloc_pptr_from_null_behaves_as_an_allocation)
{
  char **array = (char **)ReallocPPtr(2 * sizeof(char *), NULL);

  CHECK_NOT_NULL( array );
  free(array);
}

void
RegisterGUtilsTests
  (void)
{
  printf("GUtils\n");

  RUN_TEST(txt_returns_the_same_string);

  RUN_TEST(prt_error_prints_the_message_and_the_code);
  RUN_TEST(prt_error_prints_negative_codes);
  RUN_TEST(prt_dbg_error_prints_the_message_and_the_origin);

  RUN_TEST(ptr_verify_returns_a_valid_pointer_untouched);
  RUN_TEST(ptr_verify_reports_null_and_returns_null);

  RUN_TEST(alloc_ptr_returns_usable_memory);
  RUN_TEST(realloc_ptr_keeps_the_previous_content);
  RUN_TEST(realloc_ptr_from_null_behaves_as_an_allocation);

  RUN_TEST(alloc_pptr_returns_an_usable_pointer_array);
  RUN_TEST(realloc_pptr_keeps_the_previous_pointers);
  RUN_TEST(realloc_pptr_from_null_behaves_as_an_allocation);

  return;
}
