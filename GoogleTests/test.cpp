#include "pch.h"
#include "TestClass.h"

TEST(TestCaseName, TestName) {
  EXPECT_EQ(1, 1);

  TestClass testObject;
  testObject.testMethod();

  EXPECT_TRUE(true);
}