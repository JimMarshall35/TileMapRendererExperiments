#include <gtest/gtest.h>
#include <filesystem>

TEST(Events, FilePresent)
{
	ASSERT_TRUE(std::filesystem::exists("data/GameFrameworkEventTestUILayer.lua"));
	ASSERT_TRUE(std::filesystem::exists("data/GameFrameworkEventTestUILayer.xml"));
}

TEST(Events, GameToUI)
{

}